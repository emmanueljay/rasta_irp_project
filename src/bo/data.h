/** @file */

#ifndef DATA_H
#define DATA_H

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
  // drivers
  // trailers
  // bases
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


};


#endif // DATA_H