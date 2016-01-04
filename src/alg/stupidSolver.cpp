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
          starting_date+driver, 
          driver, 
          trailer,
          starting_date);
        return &(*i);
      }
    }
  }

  // If no shift existed before
  sol->shifts()->emplace_back( 
    starting_date+driver, 
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
  int driver = 0;
  int trailer = 0;

  // // Create empty shift on each time window : 
  // for (std::pair<int,Driver> const& driver : data.drivers()) 
  //   for (timeWindow const& tw : driver.second.timeWindows()) 
  //     sol.new_shift(driver.first,tw);


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
  int random = rand();
  for (int t = 0; t < data.horizon(); ++t) { 
    for (std::pair<int,Customer> const& customer : data.customers()) {
      if (sol.customers_content(customer.first)[t] < customer.second.safetyLevel() ||
          (random%100 == -1 && t*data.unit() > 1500 )) {
        VLOG(2) 
          << "We reach safety level for custumer " << customer.second.index() 
          << " at time : " << t*data.unit();

        tag = ERROR_NOT_ADMISSIBLE;
        std::string list_error(""); 
        double counter = -0.99;
        while (tag != SOLUTION_ADMISSIBLE && counter < 20.5) {

          if (sol.is_admissible(new int, new int) != SOLUTION_ADMISSIBLE) {
            LOG(ERROR) << "At this point, we have a non admissible function :";
            sol.print_trailer_content(trailer);            
            sol.print_customer_content(customer.first);
            sol.print();
            LOG(FATAL) << "Solution broken";
          }

          VLOG(3) << "-----------------------------------------------------------";
          VLOG(3) << "Finding an operation to insert -- Try number : " << counter;
          VLOG(3) << "-----------------------------------------------------------";
          
          // Find Driver/Trailer
          int num_driver = (rand() % data.drivers().size());
          auto driver_obj = data.drivers().begin();
          for (int i = 0; i < num_driver; ++i) ++driver_obj;

          driver = driver_obj->first;
          trailer = data.drivers().at(driver).trailer();
          VLOG(3) << "We found driver " << driver << " and trailer " << trailer;

          // Find/Create Shift
          int tw;
          tw = find_index_tw(data.drivers().at(driver), data.unit(), t);
          if (tw - counter >= 0) tw -= counter;

          VLOG(3) << "We found the time window " << tw << " to insert or operation ";
          Shift* shift = find_or_create_shift(
            &sol, 
            driver, 
            trailer, 
            data.drivers().at(driver).timeWindows().at(tw).first);

          VLOG(3) << "Adding on shift " << shift->index();

          // TODO : MAKE IT DO SOMETHING WHEN DRIVING TIME TO BIG
          bool clean = counter <= 0;
          tag = sol.insert_max(shift, customer.second, clean);
          VLOG(3) << "After insertion, tag value is  " 
            << rip::tags::get_string(tag);

          counter += 1.0/(data.drivers().size()*2);

          list_error += rip::tags::get_string(tag) + "\n";

          // sol.print_customer_content(customer.first);
          // sol.print_trailer_content(trailer); 
        }
        if (tag != SOLUTION_ADMISSIBLE) {
          LOG(ERROR) << list_error;
          sol.print();
          return false;
        }
      }
      random = rand();
    }
  }

  LOG(INFO) << "We found a solution !";
  sol.print();
  return true;
}