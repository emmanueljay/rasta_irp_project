#include "bo/operation.h"
#include <vector> 

class Shift {
private:
  int index_m;
  int driver_m;
  int trailer_m;
  int start_m;
  std::vector<Operation> operations_m;

public:
  Shift(int index_p, int driver_p, int trailer_p):
    index_m(index_p), driver_m(driver_p), trailer_m(trailer_p) {}
  
};