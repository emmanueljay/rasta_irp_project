/** @file */

#include "bo/driver.h"

#include <glog/logging.h>
#include <gtest/gtest.h>

/**
 * SETTER AND GETTER
 */

// Time Windows 
timeWindow* Driver::timeWindows(int i)
{
  if (i < timeWindows_m.size()) 
    return(&(timeWindows_m[i]));
  else LOG(ERROR) <<"Trying to access element out of the array of Time Windows";
  return NULL;
}