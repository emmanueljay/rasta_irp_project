/** @file */

#include "bo/shift.h"
#include <glog/logging.h>

void Shift::print() const {
  LOG(INFO) << "Shift " << index_m << " : driver " << driver_m 
    << ", trailer " << trailer_m << ", starting at " << start_m;
  for (Operation const& op : operations_m) 
    op.print();
  return;
}
