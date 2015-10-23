/** @file */

#include "bo/data.h"

#include <glog/logging.h>

/**
 * SETTER AND GETTER
 */

// Time Matrices 
int timeMatrices(int i, int j) 
{
  if (i < timeMatrices_m.size() && j<timeMatrices_m[i].size()) 
    return(timeMatrices_m[i][j]);
  else LOG(ERROR) << "Trying to access element out of the time matrix";
}

Driver* drivers(int i);
{
  if (i < drivers_m.size()) 
    return(&(drivers_m[i]));
  else LOG(ERROR) << "Trying to access element out of the array of driver";
}
