/** @file */

#ifndef OPERATION_H
#define OPERATION_H

#include <vector>

class Operation {
private:
  int point_m;
  int arrival_m;
  int quantity_m;
public:
  Operation(int point_p, int arrival_p, int quantity_p):
    point_m(point_p), arrival_m(arrival_p), quantity_m(quantity_p) {}

  bool is_admissible(
    std::vector<double> const& customer_content_p,
    double customer_max_capacity_p,
    std::vector<double> const& driver_content_p,
    double driver_max_capacity_p);
};

#endif // OPERATION_H