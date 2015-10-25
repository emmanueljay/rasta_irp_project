#include "bo/data.h"

#include <string>


class Context {
private:
  Data data_m;
  /* data */
public:
  Context(std::string const& data_instance_p);
};