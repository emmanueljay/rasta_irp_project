/** @file */

#ifndef INSTANCE_READER_H
#define INSTANCE_READER_H

#include "bo/data.h"

#include <string>


/** INSTANCE_READER
 * Reading function for the instances
 */

namespace rip {
namespace reader {

/**
 * Loading the instance .xml file
 * @param  data_p [description]
 * @param  file_p [description]
 * @return        [description]
 */
bool load_instance(Data* data_p, std::string const& file_p);


} // namespace reader
} // namespace rip

#endif // INSTANCE_READER_H