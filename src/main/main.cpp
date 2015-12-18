/** @file */

// Include Order
// 1. .h file corresponding to this cpp file (if applicable)
// 2. headers from the same component,
// 3. headers from other components,
// 4. system headers.

#include "CMakeParams.h"
#include "alg/solver.h"
#include "bo/tags.h"

#include <glog/logging.h>

#include <string>

int main(int argc, char* argv[])
{
  // Initialize Google's logging library.
  FLAGS_stderrthreshold = 0;
  FLAGS_log_dir = "../logs";
  FLAGS_v = 3; // Level of logging for debug
  google::InitGoogleLogging(argv[0]);


  /** Log Examples */

  // Debug log
  VLOG(1) << "I'm printed when you run the program with --v=1 or higher";
  VLOG(2) << "I'm printed when you run the program with --v=2 or higher";
  VLOG(3) << "I'm printed when you run the program with --v=3 or higher";

  // Common log
  LOG(INFO)
    << "Using CMake Build in " << CMAKE_BUILD_TYPE
    << " mode -- Rasta IRP Version : "
    << RASTA_IRP_VERSION_MAJOR << "."
    << RASTA_IRP_VERSION_MINOR << "."
    << RASTA_IRP_VERSION_PATCH;
  LOG(ERROR)
    << "Using CMake Build in " << CMAKE_BUILD_TYPE
    << " mode -- Rasta IRP Version : "
    << RASTA_IRP_VERSION_MAJOR << "."
    << RASTA_IRP_VERSION_MINOR << "."
    << RASTA_IRP_VERSION_PATCH;
  LOG(WARNING)
    << "Using CMake Build in " << CMAKE_BUILD_TYPE
    << " mode -- Rasta IRP Version : "
    << RASTA_IRP_VERSION_MAJOR << "."
    << RASTA_IRP_VERSION_MINOR << "."
    << RASTA_IRP_VERSION_PATCH;


  /** Instance reading test */
  if (argc < 2) 
    LOG(FATAL) << "You need an instance in parameter";
  else {
    Context context(argv[1]);
    StupidSolver solver(&context);
    solver.solve();

    // int shift_l,operation_l;
    // int tag = context.solution()->is_admissible(&shift_l,&operation_l);
    // LOG(INFO) << rip::tags::get_string(tag);
  }
    
  return 0;
}