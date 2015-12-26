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
      if (i->start() == starting_date && i->driver() == driver && i->trailer() == trailer) {
        VLOG(3) << "We found an existing shift : " << i->index();
        return &(*i);
      }
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

  // If no shift existed before
  sol->shifts()->emplace_back( 
    sol->shifts()->size(), 
    driver, 
    trailer,
    starting_date);
  return &(sol->shifts()->at(sol->shifts()->size()-1));
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
  // Going through the time finding a drop in oxygen in the customer.
  for (int t = 0; t < data.horizon(); ++t) { 
    for (std::pair<int,Customer> const& customer : data.customers()) {
  
      if (sol.customers_content(customer.first)[t] < customer.second.safetyLevel()) {
        VLOG(2) 
          << "We reach safety level for custumer " << customer.second.index() 
          << " at time : " << t*data.unit();

        // tag = ERROR_NOT_ADMISSIBLE;
        // while (tag != SOLUTION_ADMISSIBLE) {
        
        VLOG(3) << "Finding an operation to insert";
        // Find Driver/Trailer
        int driver, trailer;

        // TODO : EXTERNALISE THAT
        int num_driver = (rand() % data.drivers().size());
        auto driver_obj = data.drivers().begin();
        for (int i = 0; i < num_driver; ++i) ++driver_obj;

        driver = driver_obj->first;
        trailer = data.drivers().at(driver).trailer();
        VLOG(3) << "We found driver " << driver << " and trailer " << trailer;

        // Find/Create Shift
        int tw;
        tw = find_index_tw(data.drivers().at(driver), data.unit(), t);
        VLOG(3) << "We found the time window " << tw << " to insert or operation ";

        Shift* shift = find_or_create_shift(
          &sol, 
          driver, 
          trailer, 
          data.drivers().at(driver).timeWindows().at(tw).first);

        VLOG(3) << "Adding on shift " << shift->index();

        // TODO : MAKE IT DO SOMETHING WHEN DRIVING TIME TO BIG
        tag = sol.insert_max(shift, customer.second);
        VLOG(3) << "After insertion, tag value is  " 
          << rip::tags::get_string(tag);

        if (tag != SOLUTION_ADMISSIBLE)
          return false;
        // }
      }
    }
  }

  return true;
}