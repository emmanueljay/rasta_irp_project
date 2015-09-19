#include "CMakeParams.h"

#include <glog/logging.h>

int main(int argc, char* argv[])
{
  // Initialize Google's logging library.
  FLAGS_stderrthreshold = 0;
  FLAGS_log_dir = "../logs";
  google::InitGoogleLogging(argv[0]);

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
    
  return 0;
}