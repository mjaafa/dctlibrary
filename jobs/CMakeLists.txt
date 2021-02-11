cmake_minimum_required (VERSION 2.6)
find_package(Threads REQUIRED)

find_package(PkgConfig)
add_library(c_jobs_api STATIC job_tools.c)
target_link_libraries(c_jobs_api -lpthread)
