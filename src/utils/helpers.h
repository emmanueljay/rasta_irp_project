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
bool is_source(int point_p, Data const& data_p );


} // namespace helpers
} // namespace rip

#endif // HELPERS_H