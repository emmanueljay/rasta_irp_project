/** @file */

// Include Order
// 1. .h file corresponding to this cpp file (if applicable)
// 2. headers from the same component,
// 3. headers from other components,
// 4. system headers.

#include "utils/helpers.h"

// #include "bo/trailer.h"
// #include "bo/source.h"
// #include "bo/customer.h"

#include <glog/logging.h>


namespace rip {
namespace helpers {

bool is_source(int point_p, Data const& data_p) {
  if (data_p.sources().count(point_p) > 0) 
    return true;
  else if (data_p.customers().count(point_p) > 0) 
    return false;
  else 
    LOG(ERROR) << "The point " << point_p << " is neither in Sources or Customers";
  return false;
}

int setup_time(int point_p, Data const& data_p) {
  if (is_source(point_p,data_p))
    return data_p.sources().at(point_p).setupTime();
  else 
    return data_p.customers().at(point_p).setupTime();
}

} // namespace helpers
} // namespace rip