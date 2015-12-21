#include "alg/solver.h"

#include "bo/tags.h"

#include <glog/logging.h>

// First Way
#include <random>

// Second Way
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

/**
 * Find a timeWindow that could be used to fill the tank, before the time t
 * @param  driver That we want to give the course
 * @param  unit   Tepresent the time between t and t+1
 * @param  t      Time where it is needed to fill the tank
 * @return        The time window where we can insert the action
 */
int find_index_tw(Driver const& driver, int unit, int t) {
  for (int i = driver.timeWindows().size() - 1; i >= 0 ; --i)
  {
    if (driver.timeWindows().at(i).second < t*unit) 
      return i;
  }
  return -1;
}

Shift* find_or_create_shift(Solution* sol, int driver, int trailer, int starting_date) {
  VLOG(2) << "Finding Shift starting at " << starting_date
    << " for driver " << driver << " and trailer " << trailer;
  bool found = false;
  for (std::vector<Shift>::iterator i = sol->shifts()->begin();
       i != sol->shifts()->end();
      ++i)
  {
    if (i->start() >= starting_date) {
      // If the shift already exist
      if (i->start() == starting_date && i->driver() == driver && i->trailer() == trailer) 
        return &(*i);
      // If the driver does not correspond
      else if (i->start() == starting_date)
        continue;
      // If the shift don't exist
      else {
        // We create the shift, and return it
        VLOG(3) << "Creating shift";
        i = sol->shifts()->emplace(
          i, 
          sol->shifts()->size(), 
          driver, 
          trailer,
          starting_date);
        return &(*i);
      }
    }
  }
  sol->shifts()->emplace_back( 
    sol->shifts()->size(), 
    driver, 
    trailer,
    starting_date);
  return &(sol->shifts()->at(0));
}


bool StupidSolver::solve() {
  LOG(INFO) << name_ << " :: " << description_; 

  Data const& data = *(context_->data()); // Data structure

  Solution sol(data);
  sol.initialize();
  int tag; // To contain admissibility of the solution

  // First Way (double)
  double lower_bound = 0;
  double upper_bound = 10000;
  std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
  std::default_random_engine re;
  double a_random_double = unif(re);

  // Second Way (int)
  srand (time(NULL));
  int val = rand() % 10 + 1; // Between 1 and 10

  // Finding the time when customers are in need of oxygen.
  for (std::pair<int,Customer> const& customer : data.customers()) {
    VLOG(1) << "Treating Customer : " << customer.second.index();
    std::vector<double> const& customer_content = sol.customers_content(customer.first);
    
    // Going through the time finding a drop in oxygen in the customer.
    for (int t = 0; t < customer_content.size(); ++t)
    {
      if (customer_content[t] <= customer.second.safetyLevel()) {
        VLOG(2) 
          << "We reach safety level for custumer " << customer.second.index() 
          << " at time : " << t;

        tag = ERROR_NOT_ADMISSIBLE;
        while (tag != SOLUTION_ADMISSIBLE) {
          // Find Driver/Trailer
          int driver, trailer;
          driver = data.drivers().begin()->first; // (context_->data()->drivers()->begin() + (rand() % data.drivers().size()))->first;
          trailer = data.drivers().at(driver).trailer();

          // Find/Create Shift
          int tw;
          tw = find_index_tw(data.drivers().at(driver), data.unit(), t);

          Shift* shift = find_or_create_shift(
            &sol, 
            driver, 
            trailer, 
            data.drivers().at(driver).timeWindows().at(tw).first);

          VLOG(2) << "Adding on shift " << shift->index();

          int tag = sol.insert_max(shift, customer.second);
        }
      }
    }
  }

  return false;
}