cmake_minimum_required(VERSION 3.16)
find_package(Threads REQUIRED)
add_library(c_jobs_api STATIC job_tools.c)
target_link_libraries(c_jobs_api PRIVATE Threads::Threads)
