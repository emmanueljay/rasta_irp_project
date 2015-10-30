#include "bo/context.h"

#include "input/instance_reader.h"

#include <glog/logging.h>


Context::Context(std::string const& data_instance_p)
{
  LOG(INFO) << "Creation of The Context";
  rip::reader::load_instance(&data_m, data_instance_p);
  solution_m = Solution(data_m);
}