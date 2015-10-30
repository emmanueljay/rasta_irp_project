/** @file */

#ifndef TRAILER_H
#define TRAILER_H

class Trailer {
private:
  int index_m;
  int capacity_m;
  int initialQuantity_m;
  int distanceCost_m;

public:
  Trailer(/* args */) = default;

  // index
  void index(int index_p) {index_m = index_p;}
  int index() const {return index_m;}

  // capacity
  void capacity(int capacity_p) {capacity_m = capacity_p;}
  int capacity() const {return capacity_m;}

  // initialQuantity
  void initialQuantity(int initialQuantity_p) {initialQuantity_m = initialQuantity_p;}
  int initialQuantity() const {return initialQuantity_m;}

  // distanceCost
  void distanceCost(int distanceCost_p) {distanceCost_m = distanceCost_p;}
  int distanceCost() const {return distanceCost_m;}
};

#endif // TRAILER_H