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

#include <limits>

namespace rip {
namespace helpers {

bool is_source(int point_p, Data const& data) {
  if (data.sources().count(point_p) > 0) 
    return true;
  else if (data.customers().count(point_p) > 0) 
    return false;
  else 
    LOG(ERROR) << "The point " << point_p << " is neither in Sources or Customers";
  return false;
}

int setup_time(int point_p, Data const& data) {
  if (is_source(point_p,data))
    return data.sources().at(point_p).setupTime();
  else 
    return data.customers().at(point_p).setupTime();
}

int nearest_source(int cust_departure, int cust_arrival, Data const& data) {
  // cust_departure = bases_index if shift begin
  int min_time = std::numeric_limits<int>::max();
  int min_index;

  int time2source;
  for (auto source : data.sources()) {
    time2source = data.timeMatrices(cust_departure, source.first)
      + source.second.setupTime()
      + data.timeMatrices(source.first, cust_arrival);
    if (time2source < min_time) {
      min_time = time2source;
      min_index = source.first;
    }
  }
  VLOG(3) << "We find the nearest source from customer " << cust_departure
    << " to customer " << cust_arrival << " : " << min_index << " :: t = " << min_time;
  return min_index;
}


} // namespace helpers
} // namespace rip