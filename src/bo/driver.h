/** @file */

#ifndef DRIVER_H
#define DRIVER_H

#include <vector>
#include <utility>

typedef std::pair<int,int> timeWindow;

/**
 * 
 */
class Driver {
private:
  int index_m;
  int maxDrivingDuration_m;
  std::vector<timeWindow> timeWindows_m;
  int trailer_m;
  int minInterShiftDuration_m;
  int timeCost_m;

public:
  Driver(/* args */) = default;

  // index
  void index(int index_p) {index_m = index_p;}
  int index() const {return index_m;}

  // max driving duration
  void maxDrivingDuration(int maxDrivingDuration_p) {maxDrivingDuration_m = maxDrivingDuration_p;}
  int maxDrivingDuration() const {return maxDrivingDuration_m;}

  // Time Windows 
  std::vector<timeWindow>* timeWindows() {return &timeWindows_m;}
  timeWindow const& timeWindows(int i) const;  

  // trailer
  void trailer(int trailer_p) {trailer_m = trailer_p;}
  int trailer() const {return trailer_m;}

  // min inter shift duration
  void minInterShiftDuration(int minInterShiftDuration_p) 
    {minInterShiftDuration_m = minInterShiftDuration_p;}
  int minInterShiftDuration() const {return minInterShiftDuration_m;}

  // time cost
  void timeCost(int timeCost_p) {timeCost_m = timeCost_p;}
  int timeCost() const {return timeCost_m;}
};

#endif // DRIVER_H