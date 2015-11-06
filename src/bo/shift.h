/** @file */

#ifndef SHIFT_H
#define SHIFT_H

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
  Shift(int index_p, int driver_p, int trailer_p, int start_p):
    index_m(index_p), driver_m(driver_p), trailer_m(trailer_p), start_m(start_p) {}
  
  // Getters
  std::vector<Operation> const& operations() const {return operations_m;}
  int index() const {return index_m;}
  int driver() const {return driver_m;}
  int trailer() const {return trailer_m;}
  int start() const {return start_m;}
  
};

#endif // SHIFT_H