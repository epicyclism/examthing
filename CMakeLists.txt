cmake_minimum_required (VERSION 3.18)

set_property(GLOBAL PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project ("examthing")

find_path(SYSTEM_INCLUDE_DIR args.hxx)
include_directories(${SYSTEM_INCLUDE_DIR})
include_directories("${PROJECT_SOURCE_DIR}/common")

find_package(Boost 1.78 REQUIRED)

add_subdirectory ("common")
add_subdirectory ("examthing")
add_subdirectory ("examthingweb")
