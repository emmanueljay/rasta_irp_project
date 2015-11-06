/** @file */

#ifndef OPERATION_H
#define OPERATION_H

#include <vector>

class Operation {
private:
  // To be exported
  int point_m;
  int arrival_m;
  int quantity_m;

  // Additional information
  int departure_m;
public:
  Operation(int point_p, int arrival_p, int quantity_p,
            int departure_p):
    point_m(point_p), arrival_m(arrival_p), quantity_m(quantity_p),
    departure_m(departure_p) {}

  // Getters
  int point() const {return point_m;}
  int arrival() const {return arrival_m;}
  int departure() const {return departure_m;}
  int quantity() const {return quantity_m;}

  // On hold
  /** Constraints tested : 
   * DYN01_RESPECT_OF_TANK_CAPACITY_FOR_EACH_SITE
   * SHI05_DELIVERY_OPERATIONS_REQUIRE_THE_CUSTOMER_SITE_TO_BE_ACCESSIBLE_FOR_THE_TRAILER
   * SHI06_TRAILERQUANTITY_CANNOT_BE_NEGATIVE_OR_EXCEED_CAPACITY_OF_THE_TRAILER
   */
  // int is_admissible(
  //   std::vector<double> const& customer_content_p,
  //   double customer_max_capacity_p,
  //   std::vector<double> const& trailer_content_p,
  //   double driver_max_capacity_p);
};

#endif // OPERATION_H