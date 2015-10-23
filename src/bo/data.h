/** @file */

#ifndef DATA_H
#define DATA_H

#include "bo/driver.h"

#include <vector>

typedef std::vector<std::vector<int> > matrixInt;
/**
 * Class Data
 * 
 */
class Data {
private:
  int unit_m;
  int horizon_m;
  matrixInt timeMatrices_m;
  std::vector<Driver> drivers_m;
  // drivers
  // trailers
  int bases_index_m;
  // sources
  // customers
  // DistMatrices
public:
  Data(/* args */) = default;

  // Unit
  void unit(int unit_p) {unit_m = unit_p;}
  int unit() {return unit_m;}

  // Horizon
  void horizon(int horizon_p) {horizon_m = horizon_p;}
  int horizon() {return horizon_m;}

  // Time Matrices 
  matrixInt* timeMatrices() {return &timeMatrices_m;}
  int timeMatrices(int i, int j);

  // Drivers
  std::vector<Driver>* drivers() {return &drivers_m;}
  Driver* drivers(int i);



  // Horizon
  void bases_index(int bases_index_p) {bases_index_m = bases_index_p;}
  int bases_index() {return bases_index_m;}


};


#endif // DATA_H