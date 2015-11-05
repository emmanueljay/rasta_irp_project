#include "bo/context.h"

#include "input/instance_reader.h"

#include <glog/logging.h>

/// WARNING, just for this case TO BE REMOVED
#include "bo/tags.h"


Context::Context(std::string const& data_instance_p) : solution_m(data_m)
{
  LOG(INFO) << "Creation of The Context";
  rip::reader::load_instance(&data_m, data_instance_p);
  solution_m.initialize();
  int tag = solution_m.is_admissible();
  LOG(INFO) << rip::tags::get_string(tag);
}