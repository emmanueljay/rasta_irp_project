/** @file */

#ifndef DATA_H
#define DATA_H

#include "bo/driver.h"
#include "bo/trailer.h"
#include "bo/source.h"
#include "bo/customer.h"

#include <vector>
#include <map>

typedef std::vector< std::vector<int> > matrixInt;
typedef std::vector< std::vector<double> > matrixDouble;

/**
 * Class Data
 * 
 */
class Data {
private:
  int unit_m;
  int horizon_m;
  matrixInt timeMatrices_m;
  std::map<int,Driver> drivers_m;
  std::map<int,Trailer> trailers_m;
  int bases_index_m;
  std::map<int,Source> sources_m;
  std::map<int,Customer> customers_m;
  matrixDouble distMatrices_m;
  
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
  std::map<int,Driver>* drivers() {return &drivers_m;}
  Driver* drivers(int i);

  // Trailers
  std::map<int,Trailer>* trailers() {return &trailers_m;}
  Trailer* trailers(int i);

  // Bases 
  void bases_index(int bases_index_p) {bases_index_m = bases_index_p;}
  int bases_index() {return bases_index_m;}

  // Sources
  std::map<int,Source>* sources() {return &sources_m;}
  Source* sources(int i);

  // Customers
  std::map<int,Customer>* customers() {return &customers_m;}
  Customer* customers(int i);

  // Dist Matrices 
  matrixDouble* distMatrices() {return &distMatrices_m;}
  double distMatrices(int i, int j);

};


#endif // DATA_H