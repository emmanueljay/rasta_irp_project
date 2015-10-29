/** @file */

#ifndef SOLUTION_H
#define SOLUTION_H

#include "bo/data.h"
#include "bo/shift.h"

#include <map>
#include <vector>


class Solution {
private:
  // To be exported.
  std::vector<Shift> shifts_m;

  // Qualitative data to check if this solution is admissible.
  std::map <int, std::vector<double> > customers_content_m;
  std::map <int, std::vector<double> > drivers_content_m;
  
public:
  Solution(/* args */) = default;
  Solution(Data const& data_p);
};

#endif // SOLUTION_H