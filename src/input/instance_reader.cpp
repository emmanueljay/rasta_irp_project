/** @file */

// Include Order
// 1. .h file corresponding to this cpp file (if applicable)
// 2. headers from the same component,
// 3. headers from other components,
// 4. system headers.

#include "input/instance_reader.h"

#include "bo/driver.h"
#include "bo/trailer.h"
#include "bo/source.h"
#include "bo/customer.h"
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

/** Helper function to load dist matrix */
int load_dist_matrices(Data* data_p, rapidxml::xml_node<>* dist_mat_node_p) 
{
  VLOG(2) << "Loading Distance Matrices";
  rapidxml::xml_node<>* double_vector_node_l = dist_mat_node_p->first_node();
  matrixDouble* distMat = data_p->distMatrices();

  int dist_matrices_size = 0;
  do 
  {
    check_name(double_vector_node_l,"ArrayOfDouble");

    // Store the vector into a line of the distMatrices in data
    distMat->emplace_back(std::vector<double>());
    rapidxml::xml_node<>* child_l = double_vector_node_l->first_node();
    std::string test("");
    do {
      check_name(child_l,"double");
      distMat->at(dist_matrices_size).push_back(std::stof(child_l->value()));
      test += child_l->value();
      test += "\t";
      child_l = child_l->next_sibling();
    }
    while(child_l);
    VLOG(3) << test;

    dist_matrices_size++;
    double_vector_node_l = double_vector_node_l->next_sibling();
  }
  while(double_vector_node_l);

  // Maybe write a test to check is matrix is square !
  
  return dist_matrices_size;
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
  rapidxml::xml_node<>* trailer_node_p = trailers_node_p->first_node();
  matrixInt* timeMat = data_p->timeMatrices();

  int trailers_size = 0;
  do 
  {
    // Creating a New Trailer Object, adding it to the vector in data.
    std::vector<Trailer>* trailers_l = data_p->trailers();
    int driver_index_l = trailers_l->size();
    trailers_l->emplace_back(Trailer());
    Trailer* trailer_l = &(trailers_l->at(driver_index_l));
    VLOG(2) << "--- TRAILER " << driver_index_l << " ---";

    // Index 
    rapidxml::xml_node<> *child_l = trailer_node_p->first_node();
    check_name(child_l,"index");
    trailer_l->index(std::stoi(child_l->value()));
    VLOG(2) << "index = " << trailer_l->index();

    // Capacity
    child_l = child_l->next_sibling();
    check_name(child_l,"Capacity");
    trailer_l->capacity(std::stoi(child_l->value()));
    VLOG(2) << "Capacity = " << trailer_l->capacity();

    // Initial Quantity
    child_l = child_l->next_sibling();
    check_name(child_l,"InitialQuantity");
    trailer_l->initialQuantity(std::stoi(child_l->value()));
    VLOG(2) << "InitialQuantity = " << trailer_l->initialQuantity();

    // Distance Cost
    child_l = child_l->next_sibling();
    check_name(child_l,"DistanceCost");
    trailer_l->distanceCost(std::stoi(child_l->value()));
    VLOG(2) << "DistanceCost = " << trailer_l->distanceCost();

    trailers_size++;
    trailer_node_p = trailer_node_p->next_sibling();
  }
  while(trailer_node_p);  
  return trailers_size;
}

/** Helper function to load all sources */
int load_sources (Data* data_p, rapidxml::xml_node<>* sources_node_p)
{
  VLOG(2) << "Loading sources" ;
  rapidxml::xml_node<>* source_node_p = sources_node_p->first_node();
  matrixInt* timeMat = data_p->timeMatrices();

  int sources_size = 0;
  do 
  {
    // Creating a New Source Object, adding it to the vector in data.
    std::vector<Source>* sources_l = data_p->sources();
    int driver_index_l = sources_l->size();
    sources_l->emplace_back(Source());
    Source* source_l = &(sources_l->at(driver_index_l));
    VLOG(2) << "--- SOURCE " << driver_index_l << " ---";

    // Index 
    rapidxml::xml_node<> *child_l = source_node_p->first_node();
    check_name(child_l,"index");
    source_l->index(std::stoi(child_l->value()));
    VLOG(2) << "index = " << source_l->index();

    // Setup Time
    child_l = child_l->next_sibling();
    check_name(child_l,"setupTime");
    source_l->setupTime(std::stoi(child_l->value()));
    VLOG(2) << "setupTime = " << source_l->setupTime();

    sources_size++;
    source_node_p = source_node_p->next_sibling();
  }
  while(source_node_p);  
  return sources_size;
}

/** Helper function to load a unique drivers */
bool load_one_customer(Data* data_p, rapidxml::xml_node<>* customer_node_p) 
{
  // Creating a New Customer Object, adding it to the vector in data.
  std::vector<Customer>* customers_l = data_p->customers();
  int customer_index_l = customers_l->size();
  customers_l->emplace_back(Customer());
  Customer* customer_l = &(customers_l->at(customer_index_l));
  VLOG(2) << "--- CUSTOMER " << customer_index_l << " ---";

  // Index 
  rapidxml::xml_node<> *child_l = customer_node_p->first_node();
  check_name(child_l,"index");
  customer_l->index(std::stoi(child_l->value()));
  VLOG(2) << "Customer index = " << customer_l->index();

  // Setup Time
  child_l = child_l->next_sibling();
  check_name(child_l,"setupTime");
  customer_l->setupTime(std::stoi(child_l->value()));
  VLOG(2) << "setupTime = " << customer_l->setupTime();

  // Allowed Trailers
  child_l = child_l->next_sibling();
  int num_allowed_trailers_l = 0;
  check_name(child_l,"allowedTrailers");
  rapidxml::xml_node<>* allowed_trailers_node_l = child_l->first_node();
  int start_time_windows_l,end_time_windows_l;
  do {
    check_name(allowed_trailers_node_l,"int");
    customer_l->allowedTrailers()->push_back(std::stoi(allowed_trailers_node_l->value()));
    VLOG(3) << "Allowed Trailer  : " 
      << customer_l->allowedTrailers()->at(num_allowed_trailers_l);
    allowed_trailers_node_l = allowed_trailers_node_l->next_sibling();
    num_allowed_trailers_l++;
  }
  while(allowed_trailers_node_l);
  VLOG(2) << "Number of Allowed Trailers = " << num_allowed_trailers_l;

  // Forecast
  child_l = child_l->next_sibling();
  int num_forecast_l = 0;
  check_name(child_l,"Forecast");
  rapidxml::xml_node<>* forecast_node_l = child_l->first_node();
  customer_l->forecast()->reserve(data_p->horizon());
  do {
    check_name(forecast_node_l,"double");
    customer_l->forecast()->push_back(std::stoi(forecast_node_l->value()));
    VLOG_EVERY_N(3, 255) << "Forecast " << google::COUNTER 
      << " : " << customer_l->forecast()->at(num_forecast_l);
    forecast_node_l = forecast_node_l->next_sibling();
    num_forecast_l++;
  }
  while(forecast_node_l);
  VLOG(2) << "Size of forecast data = " << num_forecast_l;

  // Capacity
  child_l = child_l->next_sibling();
  check_name(child_l,"Capacity");
  customer_l->capacity(std::stoi(child_l->value()));
  VLOG(2) << "Capacity = " << customer_l->capacity();

  // Initial Tank Quantity
  child_l = child_l->next_sibling();
  check_name(child_l,"InitialTankQuantity");
  customer_l->initialTankQuantity(std::stoi(child_l->value()));
  VLOG(2) << "InitialTankQuantity = " << customer_l->initialTankQuantity();

  // SafetyLevel
  child_l = child_l->next_sibling();
  check_name(child_l,"SafetyLevel");
  customer_l->safetyLevel(std::stoi(child_l->value()));
  VLOG(2) << "SafetyLevel = " << customer_l->safetyLevel();

  return true;
}

/** Helper function to load all customers */
int load_customers (Data* data_p, rapidxml::xml_node<>* customers_node_p)
{
  VLOG(2) << "Loading customers" ;
  rapidxml::xml_node<>* customer_node_l = customers_node_p->first_node();

  int customers_size = 0;
  do 
  {
    if (!load_one_customer(data_p,customer_node_l))
      LOG(ERROR) << "Could't load driver " << customers_size;
    customers_size++;
    customer_node_l = customer_node_l->next_sibling();
  }
  while(customer_node_l);  
  return customers_size;
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

  // Time Matrices
  child_l = child_l->next_sibling();
  check_name(child_l,"distMatrices");
  int size_dist_matrices = load_dist_matrices(data_p, child_l);
  VLOG(1) <<  "Dist Matrix loaded of size = " << size_dist_matrices;

  // Clearing memory space
  delete[] buffer;
  return true;
}

} // namespace reader
} // namespace rip