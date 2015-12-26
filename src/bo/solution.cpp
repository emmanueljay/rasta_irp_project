/** @file */

#include "bo/solution.h"
#include "bo/tags.h"
#include "utils/helpers.h"

#include <glog/logging.h>

#include <algorithm>

void Solution::initialize()
{
  VLOG(1) << "Initializing a Solution";
  // Fill up customers content over time
  std::map<int,Customer> const& customers_l = data_m.customers();
  for (std::map<int, Customer>::const_iterator customer_it = customers_l.begin();
       customer_it != customers_l.end(); ++customer_it)
  {
    VLOG(2) << "Initializing Customer stock for Customer " << customer_it->first;
    
    /** Filling customer content (for one customer) with a vector of the size of
     * forecast : initial quantity - forecast
     **/
    customers_content_m.emplace_hint(
      customers_content_m.end(),
      customer_it->first,
      std::vector<double>(
        customer_it->second.forecast().size(),
        customer_it->second.initialTankQuantity()));

    // Filling the vector of consomation
    double consomation = 0;
    int time_index_l = 0;
    for (
      std::vector<double>::const_iterator i = customer_it->second.forecast().begin();
      i != customer_it->second.forecast().end();
      ++i)
    {
      consomation += *i;
      customers_content_m[customer_it->first][time_index_l] -= consomation;
      VLOG_EVERY_N(3, 100) << time_index_l <<"\t:\t" 
        << customers_content_m[customer_it->first][time_index_l];
      time_index_l++;
    }
  }

  // Fill up drivers content over time
  std::map<int,Driver> const& drivers_l = data_m.drivers();
  for (std::map<int, Driver>::const_iterator driver_it = drivers_l.begin();
       driver_it != drivers_l.end(); ++driver_it)
  {
    VLOG(2) << "Initializing Trailer stock for Driver " << driver_it->first;
    Trailer const& trailer_l = data_m.trailers().at(driver_it->second.trailer());
    
    /** Filling Driver content (for one driver) with a vector of the size of
     * forecast (we assume that every forecast has same size, so we take the
     * first one)
     **/
    trailers_content_m.emplace_hint(
      trailers_content_m.end(),
      driver_it->first,
      std::vector<double>(
        data_m.customers().begin()->second.forecast().size(),
        trailer_l.initialQuantity()));
  }
}

int Solution::is_admissible(int* current_shift_p, int* current_operation_p) 
{
  /**** We test the following constraints here :
   *** Drivers
   * DRI01_INTER_SHIFTS_DURATION
   *** Trailers
   * TL01_DIFFERENT_SHIFTS_OF_THE_SAME_TRAILER_CANNOT_OVERLAP_IN_TIME
   *** Customers 
   * DYN01_RESPECT_OF_TANK_CAPACITY_FOR_EACH_SITE
   *** Shifts (Thoses are about the vector of trailer's quantity)
   * SHI06_TRAILERQUANTITY_CANNOT_BE_NEGATIVE_OR_EXCEED_CAPACITY_OF_THE_TRAILER
   * SHI07_INITIAL_QUANTITY_OF_A_TRAILER_FOR_A_SHIFT_IS_THE_END_QUANTITY_OF_THE_TRAILER_FOLLOWING_THE_PREVIOUS_SHIFT
   * 
   ***** Order
   * - We test every Operation
   * - We test every Shift
   * - We test the above constraints
   *
   ***** Exception
   * We Do not test the run out avoidance, as it is the thing we are trying to construct.
   */
  
  VLOG(2) << "Testing admissibility of the solution";
  int current_tag; /// Will store the tag of a constraints if it is not satisfied

  for (std::vector<Shift>::iterator s = shifts_m.begin();
       s != shifts_m.end(); ++s)
  { 
    *current_shift_p = s - shifts_m.begin();
    VLOG(5) << "Treating Shift " << *current_shift_p;
    for (std::vector<Operation>::const_iterator o = s->operations().begin();
         o != s->operations().end(); ++o)
    {
      *current_operation_p = o - s->operations().begin();
      VLOG(5) << "Treating Operation " << *current_operation_p;
      // Check is the operation is OK
      current_tag = is_operation_admissible(*current_shift_p,*current_operation_p);
      if (current_tag != OPERATION_ADMISSIBLE) {
        VLOG(2) << "Result = " << rip::tags::get_string(current_tag);
        return current_tag;
      }
    }

    // Check is the shift is OK
    current_tag = is_shift_admissible(*current_shift_p);
    if (current_tag != SHIFT_ADMISSIBLE) {
      VLOG(2) << "Result = " << rip::tags::get_string(current_tag);
      return current_tag;
    }
  }

  VLOG(5) << "Treating constraints related to solution ";

  // DRI01_INTER_SHIFTS_DURATION
  // Start (s2) > end (s1) + MININTERSHIFTDURATION (d) OR start (s1) > end (s2) + MININTERSHIFTDURATION (d)
  int intShiftDur;  // Value of intershift duration
  for (Shift const& s1 : shifts_m)
    for (Shift const& s2 : shifts_m) {
      // If the shift are different with the sames drivers
      if ((s1.index() != s2.index()) && (s1.driver() == s2.driver())) {
        intShiftDur = data_m.drivers().at(s1.driver()).minInterShiftDuration();
        // We exit when the shifts are too close
        if ((s2.start() <= s1.end(data_m) + intShiftDur) &&
            (s1.start() <= s2.end(data_m) + intShiftDur)) {
          current_tag = DRI01_INTER_SHIFTS_DURATION;
          VLOG(2) << "Result = " << rip::tags::get_string(current_tag);
          return current_tag;
        }
      }
    }

  // TL01_DIFFERENT_SHIFTS_OF_THE_SAME_TRAILER_CANNOT_OVERLAP_IN_TIME
  // For all tl  TRAILERS For all s1, s 2  shifts(tl), start(s2) > end(s1) or start(s1) > end(s2)
  for (Shift const& s1 : shifts_m)
    for (Shift const& s2 : shifts_m) {
      // If the shift are different with the sames trailer
      if ((s1.index() != s2.index()) && (s1.trailer() == s2.trailer())) {
        // We exit if the shift overlaps
        if ((s2.start() <= s1.end(data_m)) && 
            (s1.start() <= s2.end(data_m))) {
          current_tag = TL01_DIFFERENT_SHIFTS_OF_THE_SAME_TRAILER_CANNOT_OVERLAP_IN_TIME;
          VLOG(2) << "Result = " << rip::tags::get_string(current_tag);
          return current_tag;
        }
      }
    }

  // DYN01_RESPECT_OF_TANK_CAPACITY_FOR_EACH_SITE
  // For all p CUSTOMERS, For all h  [0, H[
  //  tankQuantity(p, h) ≤ CAPACITY(p)
  //  0 <= tankQuantity(p, h) is asured by the safety level
  for (std::pair<int,Customer> const& customer : data_m.customers()) {
    for (int t = 0; t < customers_content_m[customer.first].size(); ++t)
    {
      if (customers_content_m[customer.first][t] > customer.second.capacity()) {
        current_tag = DYN01_RESPECT_OF_TANK_CAPACITY_FOR_EACH_SITE;
        VLOG(2) << "Result = " << rip::tags::get_string(current_tag);
        return current_tag;
      }
    }
  }
  // WE DON'T TEST THE FOLLOWING : (Assured by insertion functions)
  // For all p  { cCUSTOMERS } tankQuantity(p,-1) = INITIALTANKQUANTITY(p) For all h  [0, H[
  // Given that dyn = tankQuantityl(p,h-1) – FORECAST(p,h) + oOperations(p,h) quantity(o) tankQuantity(p,h) = max(dyn,0)


  // SHI06_TRAILERQUANTITY_CANNOT_BE_NEGATIVE_OR_EXCEED_CAPACITY_OF_THE_TRAILER
  // For a given shift s  SHIFTS, For all o  Operations(s) with {final(s}), 
   // trailerQuantity(o) = trailerQuantity(prev(o)) - quantity(o) 
   // trailerQuantity(o) ≥ 0
   // trailerQuantity(o) ≤ CAPACITY(trailer(s))
  // WE DON'T TEST trailerQuantity(o) = trailerQuantity(prev(o)) - quantity(o),
  // AS IT SHOULD BE MANAGED BY THE FUNCTIONS OF INSERTION.
  for (std::pair<int,Trailer> const& trailer : data_m.trailers()) {
    for (int t = 0; t < trailers_content_m[trailer.first].size(); ++t)
    {
      if (0 > trailers_content_m[trailer.first][t] ||
          trailers_content_m[trailer.first][t] > trailer.second.capacity()) {
        current_tag = SHI06_TRAILERQUANTITY_CANNOT_BE_NEGATIVE_OR_EXCEED_CAPACITY_OF_THE_TRAILER;
        VLOG(2) << "Result = " << rip::tags::get_string(current_tag) 
          << " at time " << t*data_m.unit() << " with value " 
          << trailers_content_m[trailer.first][t];
        return current_tag;
      }
    }
  }
   
  // SHI07_INITIAL_QUANTITY_OF_A_TRAILER_FOR_A_SHIFT_IS_THE_END_QUANTITY_OF_THE_TRAILER_FOLLOWING_THE_PREVIOUS_SHIFT
  // Assured by the vector of trailer_quantity
    

  VLOG(2) << "Result = " << rip::tags::get_string(SOLUTION_ADMISSIBLE);
  return SOLUTION_ADMISSIBLE;
}

int Solution::is_shift_admissible (int s)
{
  /**** We test the following constraints here :
   *** Drivers
   * DRI03_RESPECT_OF_MAXIMAL_DRIVING_TIME
   * DRI08_TIME_WINDOWS_OF_THE_DRIVERS
   *** Trailers
   * TL03_THE_TRAILER_ATTACHED_TO_A_DRIVER_IN_A_SHIFT_MUST_BE_COMPATIBLE
   *** Shift 
   * SHI02_ARRIVAL_AT_A_POINT_REQUIRES_TRAVELING_TIME_FROM_PREVIOUS_POINT (shift part)
   * (This last function is coded threw the end() function in shift.h)
   */
  std::vector<Operation> const& ops_l = shifts_m[s].operations();
  Driver const& driver_l = data_m.drivers().at(shifts_m[s].driver());
  std::vector<timeWindow> const& tws_l = driver_l.timeWindows();

  // DRI03_RESPECT_OF_MAXIMAL_DRIVING_TIME
  // For all operations o  {Operations(s)} 
  //  If operations(s) is not final(s)
  //   cumulatedDrivingTime(o) = cumulatedDrivingTime(prev(o))+timeMatrix(prev(o),o)
  //  else
  //   cumulatedDrivingTime(o) = cumulatedDrivingTime(prev(o))+ timeMatrix (o,final(s))
  // VLOG(2) << "Testing constraint : DRI03_RESPECT_OF_MAXIMAL_DRIVING_TIME";
  int cumulated_driving_time_l = 0;
  if (ops_l.size() != 0) {
    // Begin 
    cumulated_driving_time_l += 
      data_m.timeMatrices(data_m.bases_index(),ops_l.begin()->point());
    // Middle
    for (std::vector<Operation>::const_iterator o = ops_l.begin()+1;
         o != ops_l.end(); ++o)
      cumulated_driving_time_l += data_m.timeMatrices((o-1)->point(),o->point()); 
    // End
    cumulated_driving_time_l +=
      data_m.timeMatrices((ops_l.end()-1)->point(), data_m.bases_index());
  }
  if (cumulated_driving_time_l > driver_l.maxDrivingDuration())
    return DRI03_RESPECT_OF_MAXIMAL_DRIVING_TIME;

  // DRI08_TIME_WINDOWS_OF_THE_DRIVERS
  // It exists at least a tw in TIMEWINDOWS(Drivers(s)), start(s)≥start(tw) and end(tw)≥end(s)
  // VLOG(2) << "Testing constraint : DRI08_TIME_WINDOWS_OF_THE_DRIVERS";
  bool tw_found = false;
  for (std::vector<timeWindow>::const_iterator tw = tws_l.begin();
       tw != tws_l.end(); ++tw) {
    if (tw->first <= shifts_m[s].start() && shifts_m[s].end(data_m) <= tw->second)
      tw_found = true;
  }
  if (!tw_found) 
    return DRI08_TIME_WINDOWS_OF_THE_DRIVERS;

  // TL03_THE_TRAILER_ATTACHED_TO_A_DRIVER_IN_A_SHIFT_MUST_BE_COMPATIBLE
  // VLOG(2) << "Testing constraint : TL03_THE_TRAILER_ATTACHED_TO_A_DRIVER_IN_A_SHIFT_MUST_BE_COMPATIBLE";
  // trailer(s) = TRAILER(driver(s))
  if (shifts_m[s].trailer() != driver_l.trailer()) 
    return TL03_THE_TRAILER_ATTACHED_TO_A_DRIVER_IN_A_SHIFT_MUST_BE_COMPATIBLE;

  return SHIFT_ADMISSIBLE;
}

int Solution::is_operation_admissible (int s, int o)
{
  /**** We test the following constraints here :
   *** Shifts
   * SHI02_ARRIVAL_AT_A_POINT_REQUIRES_TRAVELING_TIME_FROM_PREVIOUS_POINT (operation part)
   * SHI03_LOADING_AND_DELIVERY_OPERATIONS_TAKE_A_CONSTANT_TIME
   * SHI05_DELIVERY_OPERATIONS_REQUIRE_THE_CUSTOMER_SITE_TO_BE_ACCESSIBLE_FOR_THE_TRAILER
   * SHI11_SOME_PRODUCT_MUST_BE_LOADED_OR_DELIVERED
   */
  std::vector<Operation> const& ops_l = shifts_m[s].operations();

  // SHI02_ARRIVAL_AT_A_POINT_REQUIRES_TRAVELING_TIME_FROM_PREVIOUS_POINT (operation part)
  // arrival(o) ≥ departure(prev(o)) + TIMEMATRIX(prev(o),o)
  if (o > 0)
    if (ops_l[o].arrival() 
        < ops_l[o-1].departure() 
        + data_m.timeMatrices(ops_l[o].point(),ops_l[o-1].point()))
      return SHI02_ARRIVAL_AT_A_POINT_REQUIRES_TRAVELING_TIME_FROM_PREVIOUS_POINT;

  // SHI03_LOADING_AND_DELIVERY_OPERATIONS_TAKE_A_CONSTANT_TIME
  // departure(o) = arrival(o) + SetupTime(point(o))
  int setup_time = -1; // to store the setup time
  if (rip::helpers::is_source(ops_l[o].point(),data_m)) 
    setup_time = data_m.sources().at(ops_l[o].point()).setupTime();
  else 
    setup_time = data_m.customers().at(ops_l[o].point()).setupTime();

  if (ops_l[o].departure() != ops_l[o].arrival() + setup_time)
    return SHI03_LOADING_AND_DELIVERY_OPERATIONS_TAKE_A_CONSTANT_TIME;
  
  // SHI05_DELIVERY_OPERATIONS_REQUIRE_THE_CUSTOMER_SITE_TO_BE_ACCESSIBLE_FOR_THE_TRAILER
  // For all s,o : if point(o) in CUSTOMERS then trailer(s) in ALLOWEDTRAILERS(point(o))
  if (!rip::helpers::is_source(ops_l[o].point(),data_m)) {
    std::vector<int> const& allowed_trail_l 
      = data_m.customers().at(ops_l[o].point()).allowedTrailers();
    if (std::find(allowed_trail_l.begin(),allowed_trail_l.end(), shifts_m[s].trailer())
        == allowed_trail_l.end()) {
      return SHI05_DELIVERY_OPERATIONS_REQUIRE_THE_CUSTOMER_SITE_TO_BE_ACCESSIBLE_FOR_THE_TRAILER;
    };      
  }

  // SHI11_SOME_PRODUCT_MUST_BE_LOADED_OR_DELIVERED
  if (rip::helpers::is_source(ops_l[o].point(),data_m)) {
    if (ops_l[o].quantity() >= 0) 
      return SHI11_SOME_PRODUCT_MUST_BE_LOADED_OR_DELIVERED;
  }
  else {
    if (ops_l[o].quantity() <= 0) 
      return SHI11_SOME_PRODUCT_MUST_BE_LOADED_OR_DELIVERED;
  }

  // The operation is OK
  return OPERATION_ADMISSIBLE;
}
