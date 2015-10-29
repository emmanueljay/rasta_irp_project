#include "bo/context.h"

#include "input/instance_reader.h"

Context::Context(std::string const& data_instance_p) 
{
  rip::reader::load_instance(&data_m, data_instance_p);
  solution_m = Solution(data_m);
}