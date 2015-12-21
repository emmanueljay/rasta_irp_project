/** @file */

#include "bo/solution.h"
#include "bo/tags.h"
#include "utils/helpers.h"

#include <glog/logging.h>

#include <algorithm>


int Solution::new_shift(int driver_id, int driver_work_id) {
  // TODO : Check if the id is in the map, else return false.
  VLOG(1) << "Adding shift " << shifts_m.size();
  shifts_m.emplace_back(
    shifts_m.size(), // index
    driver_id, // driver
    data_m.drivers().at(driver_id).trailer(), // trailer
    data_m.drivers().at(driver_id).timeWindows(driver_work_id).first); // Start
  return shifts_m.size()-1;
}


int Solution::insert_operation(int shift, int point_index, int arrival_time, int quantity) {
  // 0. Check if the solution is admissible to begin with :
  int err_shift, err_opeations;
  int admissibility = is_admissible(&err_shift,&err_opeations);
  if (SOLUTION_ADMISSIBLE != admissibility) {
    LOG(ERROR) << "We are inserting someting inside a solution that is not " 
      << "admissible ! : " << rip::tags::get_string(admissibility)
      << " for shift " << err_shift << " and ops " << err_opeations;
    return admissibility;
  }

  /** Insertion of the operation **/
  VLOG(2) << "Insertion of operation " << point_index << " in shift " << shift 
    << " at " << arrival_time << " delivering " << quantity;

  // 1. We find the place to insert it (we assume that the vector is already sorted ???)
  std::vector<Operation>::iterator op = shifts_m[shift].operations_ptr()->begin();
  while (op != shifts_m[shift].operations_ptr()->end() && op->departure() < arrival_time) {
    ++op;
  }

  // 2. We construct the element at this place
  int setup_time = rip::helpers::setup_time(point_index,data_m);
  op = shifts_m[shift].operations_ptr()->emplace(
    op,           // Position to emplace in the vector of operations
    point_index,  // point
    arrival_time, // arrival
    quantity,     // quantity
    setup_time    // setup_time
    );

  // 3. We check if what we inserted is ok, if not we remove the element,
  // and return back the error code
  admissibility = is_admissible(&err_shift,&err_opeations);
  if (SOLUTION_ADMISSIBLE == admissibility) {
    VLOG(2) << "Operation correctly inserted";
    // We need to update the values in solution TODO !
    // 
    // 
    // 
    // 
    return SOLUTION_ADMISSIBLE;
  }
  else {
    VLOG(2) << "Fail with code : " << rip::tags::get_string(admissibility);
    shifts_m[shift].operations_ptr()->erase(op);
    return admissibility;
  }
}

int Solution::smart_insert_operation(int shift, int point_index, int arrival_time, int quantity) {
  // Trying to insert the solution
  int tag = insert_operation(shift, point_index, arrival_time, quantity);
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






int Solution::insert_max(Shift* shift, Customer const& customer) {
  VLOG(2) << "Insertion of customer " << customer.index() << " in shift " << shift->index();
  int t = shift->end(data_m); 

  // if the truck is empty, we fill it
  // if (shift->driver())



  // Update the value of t
  // The truck is now full, put the maximum that you can in the customer, 
  // at the end of the shift using the insertion function
  

  // DONT FORGET TO UPDATES THE QUANTITIES IN SOLUTION
  

  // Return how it went.
  return 1;
}







