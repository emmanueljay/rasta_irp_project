/** @file */

#include "bo/data.h"

#include <glog/logging.h>

/**
 * SETTER AND GETTER
 */

// Time Matrices 
int Data::timeMatrices(int i, int j) 
{
  if (i < timeMatrices_m.size() && j<timeMatrices_m[i].size()) 
    return(timeMatrices_m[i][j]);
  else LOG(ERROR) << "Trying to access element out of the time matrix";
  return -1;
}

double Data::distMatrices(int i, int j) 
{
  if (i < distMatrices_m.size() && j<distMatrices_m[i].size()) 
    return(distMatrices_m[i][j]);
  else LOG(ERROR) << "Trying to access element out of the dist matrix";
  return -1.0;
}

Driver* Data::drivers(int i)
{
  if (i < drivers_m.size()) return(&(drivers_m[i]));
  else LOG(ERROR) << "Trying to access element out of the array of driver";
  return NULL;
}

Trailer* Data::trailers(int i)
{
  if (i < trailers_m.size()) return(&(trailers_m[i]));
  else LOG(ERROR) << "Trying to access element out of the array of trailer";
  return NULL;
}

Source* Data::sources(int i)
{
  if (i < sources_m.size()) return(&(sources_m[i]));
  else LOG(ERROR) << "Trying to access element out of the array of source";
  return NULL;
}

Customer* Data::customers(int i)
{
  if (i < customers_m.size()) return(&(customers_m[i]));
  else LOG(ERROR) << "Trying to access element out of the array of customer";
  return NULL;
}

