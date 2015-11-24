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
  
  LOG(INFO) << "Testing admissibility of the solution";
  int current_tag; /// Will store the tag of a constraints if it is not satisfied

  for (std::vector<Shift>::iterator s = shifts_m.begin();
       s != shifts_m.end(); ++s)
  { 
    *current_shift_p = s - shifts_m.begin();
    LOG(INFO) << "Treating Shift " << *current_shift_p;
    for (std::vector<Operation>::const_iterator o = s->operations().begin();
         o != s->operations().end(); ++o)
    {
      *current_operation_p = o - s->operations().begin();
      LOG(INFO) << "Treating Operation " << *current_operation_p;
      // Check is the operation is OK
      current_tag = is_operation_admissible(*current_shift_p,*current_operation_p);
      if (current_tag != OPERATION_ADMISSIBLE)
        return current_tag;
    }

    // Check is the shift is OK
    current_tag = is_shift_admissible(*current_shift_p);
    if (current_tag != SHIFT_ADMISSIBLE)
      return current_tag;
  }

  //check constraint DRI01|Inter-shifts duration
  
  std::map<int,Driver> const& drivers_l = data_m.drivers();
  for (std::map<int, Driver>::const_iterator driver_it = drivers_l.begin();
       driver_it != drivers_l.end(); ++driver_it)
    {
      for (std::vector<Shift>::iterator s1 = shifts_m.begin();
	   s1 != shifts_m.end(); ++s1)
	{
	  for (std::vector<Shift>::iterator s2 = shifts_m.begin();
	       s2 != shifts_m.end(); ++s2)
	    {
	      if (not((s1->start()>s2->end()+ driver_it->second.minInterShiftDuration())or(s2->start()>s1->end()+ driver_it->second.minInterShiftDuration())))
		{
		  current_tag=DRI01_INTER_SHIFTS_DURATION;
		  return current_tag;
		}
		  }
		}
	    }
    
  //check constraint TL01// different shifts of the same trailer cannot overlap
  std::map<int,Trailer> const& trailers_l=data_m.trailers();

  for(std::map<int,Trailer>::const_iterator trailer_it=trailers_l.begin();trailer_it!=trailers_l.end(); ++trailer_it)
    {
      for (std::vector<Shift>::iterator s1 = shifts_m.begin();
	   s1 != shifts_m.end(); ++s1)
	{
	  for (std::vector<Shift>::iterator s2 = shifts_m.begin();
	       s2 != shifts_m.end(); ++s2)
	    {
	      if (s1->trailer()==s2->trailer()) // there can be an overlap
		{ if (not((s2->start()>s1->end())or(s1->start()>s2->end())))
			 {
			   current_tag=DRI01_INTER_SHIFTS_DURATION;
			   return current_tag;
			 }
		 }
	     }
	}
   }
 
	  
	  

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
   */
  
  // SHI02_ARRIVAL_AT_A_POINT_REQUIRES_TRAVELING_TIME_FROM_PREVIOUS_POINT (shift part)
  // arrival(s) ≥ departure(last(Operations(s)) + TIMEMATRIX[point(last(Operations(s)),point(final(s))]

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


  // The operation is OK
  return OPERATION_ADMISSIBLE;
}
