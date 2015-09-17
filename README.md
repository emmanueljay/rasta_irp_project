# rasta_irp_project
Project for the R***** Challenge 2016

## How To

### Dependencies 

First, you need to install CMake on your computer:

    http://www.cmake.org/

Second, install CCMake (Linux, Mac OS), or CMake-Gui (Windows).
For example on Mac OS:

    brew install ccmake

### Compilation

Go into the build folder, and call cmake on the root CMakeList.txt :

    cd build
    cmake ..

Then, it should have made build files inside the build directory. If you are on MacOs, or on Linux, usually it's makefiles. On Windows, Visual Studio project files for example. If you are on Linux, just launch in the build dir:

    make

### Launch

Run the exuctable, with a parameter file as arguments (containing the instances paths) !