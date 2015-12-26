/** @file */

#ifndef HELPERS_H
#define HELPERS_H

#include "bo/data.h"

/** HELPERS
 *
 */

namespace rip {
namespace helpers {

/**
 * Check if a point is a source or a custumer (error when neither)
 * @param  point_p [description]
 * @param  Data const& data_p [description]
 * @return         [description]
 */
bool is_source(int point_p, Data const& data_p);

/**
 * Get the setup time for a source or a customer
 * @param  point_p [description]
 * @param  data_p  [description]
 * @return         [description]
 */
int setup_time(int point_p, Data const& data_p);

/**
 * Function that find the nearest source between two customers
 * @param  cust_departure Point where we start == bases_index if shift begin
 * @param  cust_arrival   Point where we want to go
 * @param  data_p         Data
 * @return                Index of the source
 */
int nearest_source(int cust_departure, int cust_arrival, Data const& data_p);

} // namespace helpers
} // namespace rip

#endif // HELPERS_H