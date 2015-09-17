#include <iostream>

#include "CMakeParams.h"


int main(int argc, char* argv[])
{
  std::cout 
    << "Using CMake Build in " << CMAKE_BUILD_TYPE
    << " mode -- Rasta IRP Version : "
    << RASTA_IRP_VERSION_MAJOR << "."
    << RASTA_IRP_VERSION_MINOR << "."
    << RASTA_IRP_VERSION_PATCH << std::endl;

  return 0;
}