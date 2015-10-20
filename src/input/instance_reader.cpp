/** @file */

// Include Order
// 1. .h file corresponding to this cpp file (if applicable)
// 2. headers from the same component,
// 3. headers from other components,
// 4. system headers.

#include "input/instance_reader.h"

// Doc for XML Parser at : http://rapidxml.sourceforge.net/manual.html
#include "utils/rapidxml.hpp" 

#include <glog/logging.h>

#include <fstream>

namespace rip {
namespace reader {

bool load_instance(Data* data_p, std::string const& file_p)
{
  LOG(INFO) << "Loading Instance inside the data structure : " << file_p;
  // VLOG(2) << "Found cookies in DEBUG";
  

  VLOG(1) << "Openning the file";
  std::ifstream file_stream_l(file_p);
  // get length of file:
  file_stream_l.seekg (0, file_stream_l.end);
  int length = file_stream_l.tellg();
  file_stream_l.seekg (0, file_stream_l.beg);
  VLOG(1) << "The length of file is : " << length;


  VLOG(1) << "Reading the file";
  char * buffer = new char [length];
  file_stream_l.read (buffer,length);
  if (file_stream_l)
    VLOG(1) << "All characters read successfully.";
  else
    LOG(FATAL) << "Error : only " << file_stream_l.gcount() << " could be read";
  file_stream_l.close();


  VLOG(1) << "Parsing the file";
  rapidxml::xml_document<> doc;    // character type defaults to char
  doc.parse<0>(buffer);    // 0 means default parse flags


  VLOG(1) << "Extracting Data";

  VLOG(2) << "Name of my first node is: " << doc.first_node()->name();
  rapidxml::xml_node<> *node = doc.first_node();
  // Attributes
  VLOG(2) << "Node has value " << node->value();
  for (rapidxml::xml_attribute<> *attr = node->first_attribute();
       attr; attr = attr->next_attribute())
  {
      VLOG(2) << "Node has attribute " << attr->name()
        << " with value " << attr->value();
  }
  // Children
  VLOG(2) << "Node has child : " << node->first_node()->name() ;
  rapidxml::xml_node<> *child = node->first_node();
  for (rapidxml::xml_attribute<> *attr = child->first_attribute();
       attr; attr = attr->next_attribute())
  {
      VLOG(2) << "Node has attribute " << attr->name()
        << " with value " << attr->value();
  } 

  // Clearing memory space
  delete[] buffer;
  return true;
}

} // namespace reader
} // namespace rip