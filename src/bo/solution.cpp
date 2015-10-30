/** @file */

#include "bo/solution.h"

#include <glog/logging.h>


Solution::Solution(Data const& data_p) 
{
  LOG(INFO) << "Initializing a Solution";
  // Fill up customers content over time
  std::map<int,Customer> const& customers_l = data_p.customers();
  for (std::map<int, Customer>::const_iterator customer_it = customers_l.begin();
       customer_it != customers_l.end(); ++customer_it)
  {
    /** Filling customer content (for one customer) with a vector of the size of
     * forecast : initial quantity - forecast
     **/
    customers_content_m.emplace_hint(
      customers_content_m.end(),
      customer_it->first,
      std::vector<double>(
        customer_it->second.forecast().size(),
        customer_it->second.initialTankQuantity()));

    // Filling the vector of consomation
    double consomation = 0;
    int time_index_l = 0;
    for (
      std::vector<double>::const_iterator i = customer_it->second.forecast().begin();
      i != customer_it->second.forecast().end();
      ++i)
    {
      consomation += *i;
      customers_content_m[customer_it->first][time_index_l] -= consomation;
      time_index_l++;
    }
  }

  // Fill up drivers content over time
  std::map<int,Driver> const& drivers_l = data_p.drivers();
  for (std::map<int, Driver>::const_iterator driver_it = drivers_l.begin();
       driver_it != drivers_l.end(); ++driver_it)
  {
    Trailer const& trailer_l = data_p.trailers().at(driver_it->second.trailer());
    
    /** Filling Driver content (for one driver) with a vector of the size of
     * forecast (we assume that every forecast has same size, so we take the
     * first one)
     **/
    drivers_content_m.emplace_hint(
      drivers_content_m.end(),
      driver_it->first,
      std::vector<double>(
        data_p.customers().begin()->second.forecast().size(),
        trailer_l.initialQuantity()));
  }
}