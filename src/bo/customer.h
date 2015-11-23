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
  double capacity_m;
  double initialTankQuantity_m;
  double safetyLevel_m;

public:
  Customer(/* args */) = default;

  // index
  void index(int index_p) {index_m = index_p;}
  int index() const {return index_m;}

  // Setup Time
  void setupTime(int setupTime_p) {setupTime_m = setupTime_p;}
  int setupTime() const {return setupTime_m;}

  // Allowed Trailers
  std::vector<int>* allowedTrailers() {return &allowedTrailers_m;}
  std::vector<int> const& allowedTrailers() const {return allowedTrailers_m;}
  int allowedTrailers(int i) const {return allowedTrailers_m.at(i);}  

  // Forecast
  std::vector<double>* forecast() {return &forecast_m;}
  std::vector<double> const& forecast() const {return forecast_m;}
  double forecast(int i) const {return forecast_m.at(i);}  

  // Capacity
  void capacity(double capacity_p) {capacity_m = capacity_p;}
  double capacity() const {return capacity_m;}

  // Initial Tank Quantity
  void initialTankQuantity(double initialTankQuantity_p) {initialTankQuantity_m = initialTankQuantity_p;}
  double initialTankQuantity() const {return initialTankQuantity_m;}

  // SafetyLevel
  void safetyLevel(double safetyLevel_p) {safetyLevel_m = safetyLevel_p;}
  double safetyLevel() const {return safetyLevel_m;}

};

#endif // CUSTOMER_H
