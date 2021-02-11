cmake_minimum_required (VERSION 2.6)

find_package(PkgConfig)
add_library(c_matrix_api STATIC matrix_tools.c)
target_link_libraries(c_matrix_api)
