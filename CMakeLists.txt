# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("Multiprecision Competency Test")
set(CMAKE_CXX_STANDARD 17)

# Include sub-projects.
add_subdirectory ("Test1")
add_subdirectory ("Test2")
