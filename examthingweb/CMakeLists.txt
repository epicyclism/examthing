cmake_minimum_required (VERSION 3.18)

add_executable (examthingweb examthingweb.cpp defaultHTML.h etw_args.cpp etw_args.h)
target_link_libraries(examthingweb common)
