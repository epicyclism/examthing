# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.18)
set_property(GLOBAL PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("examthing")

include_directories("${PROJECT_SOURCE_DIR}/common")

add_subdirectory ("common")
add_subdirectory ("examthing")
