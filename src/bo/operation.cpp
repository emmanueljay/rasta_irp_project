/** @file */

#include "bo/operation.h"
#include <glog/logging.h>


void Operation::print() const {
  LOG(INFO) << "Operation at " << point_m << " : arrival at " << arrival_m 
    << ", delivering quantity " << quantity_m << ", leaving at " << departure_m;
  return;
}
