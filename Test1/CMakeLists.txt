# CMakeList.txt : CMake project for Multiprecision Competency Test, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(Boost 1.72.0 REQUIRED)
add_executable (TestOne "TestOne.cpp")
target_include_directories(TestOne PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(TestOne PRIVATE ${Boost_LIBRARIES})

# TODO: Add tests and install targets if needed.
