/** @file */

#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <vector>

class Customer {
private:
  int index_m;
  int setupTime_m;
  std::vector<int> allowedTrailers_m;
  std::vector<double> forecast_m;
  int capacity_m;
  int initialTankQuantity_m;
  int safetyLevel_m;

public:
  Customer(/* args */) = default;

  // index
  void index(int index_p) {index_m = index_p;}
  int index() {return index_m;}

  // Setup Time
  void setupTime(int setupTime_p) {setupTime_m = setupTime_p;}
  int setupTime() {return setupTime_m;}

  // Allowed Trailers
  std::vector<int>* allowedTrailers() {return &allowedTrailers_m;}
  int allowedTrailers(int i) {return allowedTrailers_m.at(i);}  

  // Forecast
  std::vector<double>* forecast() {return &forecast_m;}
  double forecast(int i) {return forecast_m.at(i);}  

  // Capacity
  void capacity(int capacity_p) {capacity_m = capacity_p;}
  int capacity() {return capacity_m;}

  // Initial Tank Quantity
  void initialTankQuantity(int initialTankQuantity_p) {initialTankQuantity_m = initialTankQuantity_p;}
  int initialTankQuantity() {return initialTankQuantity_m;}

  // SafetyLevel
  void safetyLevel(int safetyLevel_p) {safetyLevel_m = safetyLevel_p;}
  int safetyLevel() {return safetyLevel_m;}

};

#endif // CUSTOMER_H