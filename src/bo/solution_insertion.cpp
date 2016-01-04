/** @file */

#include "bo/solution.h"
#include "bo/tags.h"
#include "utils/helpers.h"

#include <glog/logging.h>

#include <algorithm>


void Solution::update_containers(int trailer, Operation const& op, bool reverse)
{
  int fict_quantity = reverse ? -op.quantity() : op.quantity();
  bool is_customer = !rip::helpers::is_source(op.point(), data_m);

  // // Display
  // print_trailer_content(trailer);
  // if (is_customer) print_customer_content(op.point());

  // Update
  for (int t = (op.arrival())/data_m.unit();
       t < trailers_content_m[trailer].size(); ++t)
  {
    trailers_content_m[trailer][t] -= fict_quantity;
    if (is_customer) {
      customers_content_m[op.point()][t] += fict_quantity;
    }
  }

  // // Display
  // print_trailer_content(trailer);
  // if (is_customer) print_customer_content(op.point());

  return;
}

void Solution::delete_operation(Shift* shift, Operation* op) {
  LOG(INFO) << "Deleting operation : ";
  op->print();
  for (std::vector<Operation>::iterator i = shift->operations_ptr()->begin();
       i != shift->operations_ptr()->end(); 
       ++i)  
  {
    i->print();
    if (i->point() == op->point() && 
        i->arrival() == op->arrival() &&
        i->quantity() == op->quantity()) {
      // Updating the vectors of informations
      update_containers(shift->trailer(), *op, true); // Reverse updating
      shift->operations_ptr()->erase(i);
      if (shift->operations_ptr()->size() == 0)
        delete_shift(shift);
      return;
    }
  }
  LOG(ERROR) << "This Operation was not found : ";
  op->print();
  LOG(ERROR) << "In the shift : ";
  shift->print();
  LOG(FATAL) << "No Operation Found ";
  return;
}


void Solution::delete_shift(Shift* shift) {
  LOG(INFO) << "Deleting shift " << shift->index();
  shift->print();
  for (std::vector<Shift>::iterator i = shifts_m.begin();
       i != shifts_m.end(); 
       ++i)
  {
    if (i->index() == shift->index()){
      shifts_m.erase(i);
      return;
    }
  }
  return;
}

int Solution::new_shift(int driver_id, timeWindow const& tw) {
  // TODO : Check if the id is in the map, else return false.
  VLOG(1) << "Adding shift " << shifts_m.size();
  shifts_m.emplace_back(
    tw.first + driver_id, // index
    driver_id, // driver
    data_m.drivers().at(driver_id).trailer(), // trailer
    tw.first); // Start
  return tw.first + driver_id;
}


int Solution::insert_operation(Shift* shift, int point_index, int arrival_time, int quantity, bool test_admissibility) {
  // 0. Check if the solution is admissible to begin with :
  int err_shift, err_opeations;

  int admissibility = SOLUTION_ADMISSIBLE;
  if (test_admissibility) {
    admissibility = is_admissible(&err_shift,&err_opeations);
    
    if (SOLUTION_ADMISSIBLE != admissibility) {
      LOG(ERROR) << "We are inserting someting inside a solution that is not " 
        << "admissible ! : " << rip::tags::get_string(admissibility)
        << " for shift " << shifts_m[err_shift].index() << " and ops " 
        << err_opeations;

      if (shift->operations().size()== 0) 
        delete_shift(shift);
      
      return admissibility;
    }
  }

  /** Insertion of the operation **/
  VLOG(2) << "Insertion of operation " << point_index << " in shift " << shift->index() 
    << " at " << arrival_time << " delivering " << quantity;

  // 1. We find the place to insert it (we assume that the vector is already sorted ???)
  std::vector<Operation>::iterator op = shift->operations_ptr()->begin();
  while (op != shift->operations_ptr()->end() && op->departure() < arrival_time) {
    ++op;
  }

  // 2. We construct the element at this place
  int setup_time = rip::helpers::setup_time(point_index,data_m);
  VLOG(2) << "We insert the operation between " << arrival_time << " and " 
    << arrival_time + setup_time;
  op = shift->operations_ptr()->emplace(
    op,           // Position to emplace in the vector of operations
    point_index,  // point
    arrival_time, // arrival
    quantity,     // quantity
    setup_time    // setup_time
    );

  // 3. We update the values
  update_containers(shift->trailer(), *op);
  VLOG(2) << "Solution correctly updated";

  // 4. We check if what we inserted is ok, if not we remove the element,
  // and return back the error code
  if (test_admissibility) {
    admissibility = is_admissible(&err_shift,&err_opeations);
    if (SOLUTION_ADMISSIBLE == admissibility) {
      VLOG(2) << "Operation correctly inserted";

      // // 4.We need to update the values in solution !
      // update_containers(shift->trailer(), *op);

      return SOLUTION_ADMISSIBLE;
    }
    else {
      VLOG(2) << "Fail with code : " << rip::tags::get_string(admissibility);
      delete_operation(shift, &(*op));
      // update_containers(shift->trailer(), *op, true);
      // shift->operations_ptr()->erase(op);
      // if (shift->operations().size()== 0) 
      //   delete_shift(shift);
      return admissibility;
    }
  }
  else {
    // // 4.We need to update the values in solution !
    // update_containers(shift->trailer(), *op);
      
    // VLOG(2) << "Solution correctly updated";
    return is_admissible(&err_shift,&err_opeations);   
  }
}

int Solution::smart_insert_operation(int shift, int point_index, int arrival_time, int quantity) {
  // Trying to insert the solution
  int tag = insert_operation(&(shifts_m[shift]), point_index, arrival_time, quantity);
  VLOG(3) << rip::tags::get_string(tag);

  switch (tag) 
  {
    case SOLUTION_ADMISSIBLE : {
      VLOG(2) << "Operation correctly inserted";
      return SOLUTION_ADMISSIBLE;
      break;
    }
    case DRI01_INTER_SHIFTS_DURATION:
    case TL03_THE_TRAILER_ATTACHED_TO_A_DRIVER_IN_A_SHIFT_MUST_BE_COMPATIBLE:
    {
      LOG(ERROR) << "The solution is not admissible to begin with ! Problem indep from the insertion ";
      return ERROR_NOT_ADMISSIBLE;
      break;
    }
    case DRI03_RESPECT_OF_MAXIMAL_DRIVING_TIME: 
    case DRI08_TIME_WINDOWS_OF_THE_DRIVERS:
    case TL01_DIFFERENT_SHIFTS_OF_THE_SAME_TRAILER_CANNOT_OVERLAP_IN_TIME:
    {
      VLOG(2) << "Needs to try another shift !";
      return TODO_MACRO;
      break;
    }    
    case SHI02_ARRIVAL_AT_A_POINT_REQUIRES_TRAVELING_TIME_FROM_PREVIOUS_POINT:
    {
      VLOG(2) << "Needs to espace the operations, or changing the time of insertion !";
      return TODO_MACRO;
      break;
    }
    case DYN01_RESPECT_OF_TANK_CAPACITY_FOR_EACH_SITE:
    case SHI03_LOADING_AND_DELIVERY_OPERATIONS_TAKE_A_CONSTANT_TIME:
    case SHI06_TRAILERQUANTITY_CANNOT_BE_NEGATIVE_OR_EXCEED_CAPACITY_OF_THE_TRAILER:
    {
      VLOG(2) << "Needs to change the value of quantity !";
      return TODO_MACRO;
      break;
    }
    case SHI05_DELIVERY_OPERATIONS_REQUIRE_THE_CUSTOMER_SITE_TO_BE_ACCESSIBLE_FOR_THE_TRAILER:
    {
      VLOG(2) << "Try another driver !";
      return TODO_MACRO;
      break;
    }
    case SHI11_SOME_PRODUCT_MUST_BE_LOADED_OR_DELIVERED:
    {
      LOG(ERROR) << "Wrong value of quantity !";
      return ERROR_NOT_ADMISSIBLE;
      break;
    }
    case SHI07_INITIAL_QUANTITY_OF_A_TRAILER_FOR_A_SHIFT_IS_THE_END_QUANTITY_OF_THE_TRAILER_FOLLOWING_THE_PREVIOUS_SHIFT:
    {
      LOG(ERROR) << "Coding problem, should be treated by default !";
      return ERROR_NOT_ADMISSIBLE;
      break;
    }
  }
  LOG(ERROR) << "Tag not defined !";
  return ERROR_NOT_ADMISSIBLE;
}


int Solution::insert_max(Shift* shift, Customer const& customer, bool clean) {
  VLOG(2) << "Insertion of customer " << customer.index() << " in shift " << shift->index();
  
  int tag;              // Tag value
  int t;                // Value of time at the end of the shift
  int initial_position; // Position of the end of the shift
  bool refill = false;

  // Content printage
  int trailer_id, customer_id;
  trailer_id = shift->trailer();
  customer_id = customer.index();
  print_customer_content(customer_id);
  print_trailer_content(trailer_id);

  // 0. Getting initial values
  if (shift->operations().size() == 0) {
    t = shift->start();
    initial_position = data_m.bases_index();
  }
  else {
    t = shift->operations().rbegin()->departure();
    initial_position = shift->operations().rbegin()->point();
  }
  VLOG(3) << "Time of insertion : " << t << " at position " << initial_position;

  // 1. If the truck is empty, we fill it
  Trailer const& trailer = data_m.trailers().at(shift->trailer());
  if (trailers_content_m[trailer.index()][t/data_m.unit()] < trailer.capacity()/2.0 && // Arbitrary
        (customer.capacity() - customers_content_m[customer.index()][t/data_m.unit()] > 
        trailers_content_m[trailer.index()][t/data_m.unit()])) { 
    VLOG(3) << "Addition of an operation to fill the truck, because it is law in quantity : "
      << trailers_content_m[trailer.index()][t/data_m.unit()] << " at time " << t/data_m.unit();
    // Getting the source the most interesting
    Source const& source = data_m.sources().at(
      rip::helpers::nearest_source(initial_position, customer.index(), data_m));
    
    t += data_m.timeMatrices(initial_position,source.index());

    int quantity = std::min(
      static_cast<double>(trailer.capacity()),
      customer.capacity() - customers_content_m[customer.index()][t/data_m.unit()]);

    // if there are no operation after, we can do what we want.
    if (clean)
      quantity = trailer.capacity();

    // Filling the truck
    tag = insert_operation(
      shift, 
      source.index(), 
      t, 
      trailers_content_m[trailer.index()][t/data_m.unit()] - quantity,
      false); // We don't test for admissibility

    // Updating variables
    t += source.setupTime();
    initial_position = source.index();
    refill = true;
    VLOG(3) << "Time of insertion : " << t << " at position " << initial_position;
  }

  // 2. Update the value of t
  // The truck is now full, put the maximum that you can in the customer, 
  // at the end of the shift using the insertion function
  
  t += data_m.timeMatrices(initial_position,customer.index());
  
  // Filling the customer with the maxmimum possible
  int quantity = std::min(
    trailers_content_m[trailer.index()][t/data_m.unit()],
    customer.capacity() - customers_content_m[customer.index()][t/data_m.unit()]);

  int size = shift->operations_ptr()->size(); // redondant

  tag = insert_operation(
    shift, 
    customer.index(), 
    t, 
    quantity);

  // Removal of the first insertion if the second did not go correctly
  if (refill && tag != SOLUTION_ADMISSIBLE && size > 0) {  // Size redondant
    delete_operation(shift, &*(shift->operations_ptr()->rbegin()));
  }

  return tag;
}







