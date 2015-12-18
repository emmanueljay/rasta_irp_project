/** @file */

#ifndef BRICKS_H
#define BRICKS_H

#include "bo/context.h"

/** BRIKS
 * Algorithm function that do all sort of things
 */

namespace rip {
namespace alg_bricks {

/**
 * The job of this function is to optimize a shift, with respect to the 
 * constraints : 
 * This is a simple version
 * @param  data_p [description]
 * @param  file_p [description]
 * @return        [description]
 */
bool optimise_shift_simple(Context* context, int shift);


} // namespace alg_bricks
} // namespace rip

#endif // BRICKS_H