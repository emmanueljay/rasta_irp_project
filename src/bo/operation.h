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
            int delivery_time_p):
    point_m(point_p),
    arrival_m(arrival_p), 
    quantity_m(quantity_p),
    departure_m(arrival_p+delivery_time_p) {}

  // Getters
  int point() const {return point_m;}
  int arrival() const {return arrival_m;}
  int departure() const {return departure_m;}
  int quantity() const {return quantity_m;}

  void print() const;
};

#endif // OPERATION_H