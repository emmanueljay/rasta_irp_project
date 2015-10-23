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

/** To check is the name of the node correspond to the right one */
bool check_name(rapidxml::xml_node<>* node_p, std::string const& name_p)
{
  if (std::string(node_p->name()) != name_p) {
    LOG(ERROR) 
      << "Wrong Instance node, should be "<< name_p << " : " << node_p->name(); 
    return false;
  }
  else 
    return true;
}

/** Helper function to load time matrix */
int load_time_matrices(Data* data_p, rapidxml::xml_node<>* time_mat_node_p) 
{
  VLOG(2) << "Loading Time Matrices";
  rapidxml::xml_node<>* int_vector_node_l = time_mat_node_p->first_node();
  matrixInt* timeMat = data_p->timeMatrices();

  int time_matrices_size = 0;
  do 
  {
    check_name(int_vector_node_l,"ArrayOfInt");

    // Store the vector into a line of the timeMatrices in data
    timeMat->emplace_back(std::vector<int>());
    rapidxml::xml_node<>* child_l = int_vector_node_l->first_node();
    std::string test("");
    do {
      check_name(child_l,"int");
      timeMat->at(time_matrices_size).push_back(std::stoi(child_l->value()));
      test += child_l->value();
      test += "\t";
      child_l = child_l->next_sibling();
    }
    while(child_l);
    VLOG(3) << test;

    time_matrices_size++;
    int_vector_node_l = int_vector_node_l->next_sibling();
  }
  while(int_vector_node_l);

  // Maybe write a test to check is matrix is square !
  
  return time_matrices_size;
}


/** Helper function to load a unique drivers */
bool load_one_driver(Data* data_p, rapidxml::xml_node<>* driver_node_p) 
{
  // Creating a New Driver Object, adding it to the vector in data.
  std::vector<Driver>* drivers_l = data_p->drivers();
  int driver_index_l = drivers_l->size();
  drivers_l->emplace_back(Driver());
  Driver* driver_l = &(drivers_l->at(driver_index_l));
  VLOG(2) << "--- DRIVER " << driver_index_l << " ---";

  // Index 
  rapidxml::xml_node<> *child_l = driver_node_p->first_node();
  check_name(child_l,"index");
  driver_l->index(std::stoi(child_l->value()));
  VLOG(2) << "Driver index = " << driver_l->index();

  // Max Driving Duration
  child_l = child_l->next_sibling();
  check_name(child_l,"maxDrivingDuration");
  driver_l->maxDrivingDuration(std::stoi(child_l->value()));
  VLOG(2) << "maxDrivingDuration = " << driver_l->maxDrivingDuration();

  // Time Windows
  child_l = child_l->next_sibling();
  int num_time_windows_l = 0;
  check_name(child_l,"timewindows");
  rapidxml::xml_node<>* time_wind_node_l = child_l->first_node();
  int start_time_windows_l,end_time_windows_l;
  do {
    check_name(time_wind_node_l,"TimeWindow");
    driver_l->timeWindows()->push_back(std::make_pair(
      std::stoi(time_wind_node_l->first_node()->value()),
      std::stoi(time_wind_node_l->last_node()->value())
    ));
    VLOG_EVERY_N(3, 10) << "Time Window " << google::COUNTER 
      << " : " << driver_l->timeWindows()->at(num_time_windows_l).first
      << " - " << driver_l->timeWindows()->at(num_time_windows_l).second;
    time_wind_node_l = time_wind_node_l->next_sibling();
    num_time_windows_l++;
  }
  while(time_wind_node_l);
  VLOG(2) << "Number of Time Window = " << num_time_windows_l;

  // Trailer
  child_l = child_l->next_sibling();
  check_name(child_l,"trailer");
  driver_l->trailer(std::stoi(child_l->value()));
  VLOG(2) << "trailer = " << driver_l->trailer();

  // Min Inter Shift Duration
  child_l = child_l->next_sibling();
  check_name(child_l,"minInterSHIFTDURATION");
  driver_l->minInterShiftDuration(std::stoi(child_l->value()));
  VLOG(2) << "minInterShiftDuration = " << driver_l->minInterShiftDuration();

  // Time Cost
  child_l = child_l->next_sibling();
  check_name(child_l,"TimeCost");
  driver_l->timeCost(std::stoi(child_l->value()));
  VLOG(2) << "timeCost = " << driver_l->timeCost();

  return true;
}

/** Helper function to load Drivers */
int load_drivers(Data* data_p, rapidxml::xml_node<>* drivers_node_p) 
{
  VLOG(2) << "Loading Drivers";
  rapidxml::xml_node<>* driver_node_l = drivers_node_p->first_node();
  matrixInt* timeMat = data_p->timeMatrices();

  int drivers_size = 0;
  do 
  {
    if (!load_one_driver(data_p,driver_node_l))
      LOG(ERROR) << "Could't load driver " << drivers_size;
    drivers_size++;
    driver_node_l = driver_node_l->next_sibling();
  }
  while(driver_node_l);  
  return drivers_size;
}

/** Helper function to load all trailers */
int load_trailers (Data* data_p, rapidxml::xml_node<>* trailers_node_p)
{
  VLOG(2) << "Loading trailers" ;
  VLOG(2) << " TODO !";
  return 0;
}

/** Helper function to load all sources */
int load_sources (Data* data_p, rapidxml::xml_node<>* sources_node_p)
{
  VLOG(2) << "Loading sources" ;
  VLOG(2) << " TODO !";
  return 0;
}

/** Helper function to load all customers */
int load_customers (Data* data_p, rapidxml::xml_node<>* customers_node_p)
{
  VLOG(2) << "Loading customers" ;
  VLOG(2) << " TODO !";
  return 0;
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
  rapidxml::xml_node<> *child_l = root->first_node();
  check_name(child_l,"unit");
  data_p->unit(std::stoi(child_l->value()));
  VLOG(1) << "Unit = " << data_p->unit();

  // Horizon
  child_l = child_l->next_sibling();
  check_name(child_l,"horizon");
  data_p->horizon(std::stoi(child_l->value()));
  VLOG(1) << "Horizon = " << data_p->horizon();

  // Time Matrices
  child_l = child_l->next_sibling();
  check_name(child_l,"timeMatrices");
  int size_time_matrices = load_time_matrices(data_p, child_l);
  VLOG(1) <<  "Time Matrix loaded of size = " << size_time_matrices;

  // Drivers
  child_l = child_l->next_sibling();
  check_name(child_l,"drivers");
  int num_drivers = load_drivers(data_p, child_l);
  VLOG(1) <<  "Vector of driverd loaded of size = " << num_drivers;

  // Trailers
  child_l = child_l->next_sibling();
  check_name(child_l,"trailers");
  int num_trailers = load_trailers(data_p, child_l);
  VLOG(1) <<  "Vector of trailers loaded of size = " << num_trailers;

  // Bases
  child_l = child_l->next_sibling();
  check_name(child_l,"bases");
  data_p->bases_index(std::stoi(child_l->first_node()->value()));
  VLOG(1) << "bases = " << data_p->bases_index();

  // Sources
  child_l = child_l->next_sibling();
  check_name(child_l,"sources");
  int num_sources = load_sources(data_p, child_l);
  VLOG(1) <<  "Vector of sources loaded of size = " << num_sources;

  // Customers
  child_l = child_l->next_sibling();
  check_name(child_l,"customers");
  int num_customers = load_customers(data_p, child_l);
  VLOG(1) <<  "Vector of customers loaded of size = " << num_customers;

  // Clearing memory space
  delete[] buffer;
  return true;
}

} // namespace reader
} // namespace rip