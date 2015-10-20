/** @file */

// Include Order
// 1. .h file corresponding to this cpp file (if applicable)
// 2. headers from the same component,
// 3. headers from other components,
// 4. system headers.

#include "input/instance_reader.h"

#include <glog/logging.h>


namespace rip {
namespace reader {

bool load_instance(Data* data_p, std::string const& file_p)
{
  LOG(INFO) << "Loading Instance inside the data structure : " << file_p;
  // VLOG(2) << "Found cookies in DEBUG";
  
  
  return true;
}

} // namespace reader
} // namespace rip