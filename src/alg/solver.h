#ifndef SOLVER_H
#define SOLVER_H

#include "bo/context.h"
#include <iostream>

// General solver class
class Solver {
protected:
  std::string name_;
  std::string description_;

  Context* context_;
  bool found_;

public:
  Solver(Context* context) : context_(context), found_(false) {};
  ~Solver() {} ;

  // The principal methode to define
  bool solve();

  // Setters and getters
  std::string const& name() const {return name_;}
  void name(std::string name) {name_ = name;}
  
  std::string const& description() const {return description_;}
  void description(std::string description) {description_ = description;}

  Context* context() {return context_;}
  Context const& context() const {return *context_;}

  bool found() const {return found_;}
  void found(bool found) {found_ = found;}
};

/**
 * First Solver : the one that only give an admissible solution
 */
class StupidSolver : public Solver {
public:
  StupidSolver(Context* context): 
    Solver(context) {
      name("Stupid Solver");
      description("First version of a solver that need to find an admissible sol");
    }
  ~StupidSolver() {} ;

  /** 
   * Find the times where an operation is needed, and affect it to a driver 
   * in a shift 
   **/
  bool solve();
};




#endif // SOLVER_H
 