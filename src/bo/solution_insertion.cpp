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
    return SOLUTION_ADMISSIBLE;
  }
  else {
    VLOG(2) << "Fail with code : " << rip::tags::get_string(admissibility);
    shifts_m[shift].operations_ptr()->erase(op);
    return admissibility;
  }
}
