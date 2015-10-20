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

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
// ESPACE POUR DEFINIR DES HELPERS (FCT DONT ON NE SERT PAS AILLEURS) //
////////////////////////////////////////////////////////////////////////

namespace {

/** Helper function to load time matrix */
int load_time_matrices(Data* data_p, rapidxml::xml_node<>* time_mat_node_p) 
{
  VLOG(2) << "Loading Time Matrices";
  rapidxml::xml_node<>* int_vector_node_l = time_mat_node_p->first_node();
  matrixInt* timeMat = data_p->timeMatrices();

  int time_matrices_size = 0;
  do 
  {
    if (std::string(int_vector_node_l->name()) != "ArrayOfInt") 
      LOG(ERROR) 
        << "Wrong Instance node, should be ArrayOfInt : " 
        << int_vector_node_l->name();

    // Store the vector into a line of the timeMatrices in data
    timeMat->emplace_back(std::vector<int>());
    rapidxml::xml_node<>* child_l = int_vector_node_l->first_node();
    std::string test("");
    do {
      if (std::string(child_l->name()) != "int") 
        LOG(ERROR) << "Wrong Instance node, should be int : "<< child_l->name();      
      timeMat->at(time_matrices_size).push_back(std::stoi(child_l->value()));
      test += child_l->value();
      test += "\t";
      child_l = child_l->next_sibling();
    }
    while(child_l);
    VLOG(2) << test;

    time_matrices_size++;
    int_vector_node_l = int_vector_node_l->next_sibling();
  }
  while(int_vector_node_l);

  // Maybe write a test to check is matrix is square !
  
  return time_matrices_size;
}



} // namespace

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////
// ESPACE POUR DEFINIR LES FONCTIONS EXPORTEES  //
//////////////////////////////////////////////////

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

  rapidxml::xml_node<> *root = doc.first_node(); // Node IRP_R_C_I

  /** Extraction of the tree ! */

  // Unit 
  rapidxml::xml_node<> *child = root->first_node();
  if (std::string(child->name()) != "unit") 
    LOG(FATAL) << "Wrong Instance node, should be unit : " << child->name();
  data_p->unit(std::stoi(child->value()));
  VLOG(1) << "Unit = " << data_p->unit();

  // Horizon
  child = child->next_sibling();
  if (std::string(child->name()) != "horizon") 
    LOG(FATAL) << "Wrong Instance node, should be horizon : " << child->name();
  data_p->horizon(std::stoi(child->value()));
  VLOG(1) << "Horizon = " << data_p->horizon();

  // Time Matrices
  child = child->next_sibling();
  if (std::string(child->name()) != "timeMatrices") 
    LOG(FATAL) << "Wrong Instance node, should be timeMatrices : " << child->name();
  int size_time_matrices = load_time_matrices(data_p, child);
  VLOG(1) <<  "Time Matrix loaded of size = " << size_time_matrices;





  // Clearing memory space
  delete[] buffer;
  return true;
}

} // namespace reader
} // namespace rip