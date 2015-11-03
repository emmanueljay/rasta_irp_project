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
  std::map <int, std::vector<double> > trailers_content_m;
  Data const& data_m;

public:
  // Constructors
  Solution(Data const& data_p) : data_m(data_p) {}
  void initialize();

  // Admssibility checks
  int is_admissible ();
  int is_shift_admissible (int s);
  int is_operation_admissible (int s, int o);
};

#endif // SOLUTION_H