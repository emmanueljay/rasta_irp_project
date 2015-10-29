/** @file */

#ifndef CONTEXT_H
#define CONTEXT_H

#include "bo/data.h"
#include "bo/solution.h"

#include <string>


class Context {
private:
  Data data_m;
  Solution solution_m;
  /* data */
public:
  Context(std::string const& data_instance_p);
};

#endif // CONTEXT_H