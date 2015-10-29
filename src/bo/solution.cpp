/** @file */

#include "bo/solution.h"

Solution::Solution(Data const& data_p) 
{
  // Fill up customers content over time
  std::map<int,Customer>* customers_l = data_p.customers();


  // Fill up drivers content over time
  std::map<int,Driver>* drivers_l = data_p.drivers();
  // for (std::map<int, Driver>::iterator driver_l = drivers_l->begin();
  //      driver_l != drivers_l->end(); ++driver_l)
  // {
    
  // }

}