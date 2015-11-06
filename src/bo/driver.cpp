/** @file */

#include "bo/driver.h"

#include <glog/logging.h>
#include <gtest/gtest.h>

/**
 * SETTER AND GETTER
 */

// Time Windows 
timeWindow const& Driver::timeWindows(int i) const
{
  if (i < timeWindows_m.size()) 
    return(timeWindows_m[i]);
  else LOG(ERROR) <<"Trying to access element out of the array of Time Windows";
  return std::make_pair(-1,-1);
}