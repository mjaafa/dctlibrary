cmake_minimum_required (VERSION 2.6)

find_package(PkgConfig)
find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})

add_library(c_matrix_api STATIC matrix_tools.c)
target_link_libraries(c_matrix_api ${OpenCL_LIBRARIES})
