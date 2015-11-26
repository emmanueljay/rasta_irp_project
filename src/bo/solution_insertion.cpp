/** @file */

#include "bo/solution.h"
#include "bo/tags.h"
#include "utils/helpers.h"

#include <glog/logging.h>

#include <algorithm>


bool Solution::new_shift(int driver_id, int driver_work_id) {
  // TODO : Check if the id is in the map, else return false.
  
  shifts_m.emplace_back(
    shifts_m.size(), // index
    driver_id, // driver
    data_m.drivers().at(driver_id).trailer(), // trailer
    data_m.drivers().at(driver_id).timeWindows(driver_work_id).first); // Start
  return true;
}