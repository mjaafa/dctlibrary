cmake_minimum_required (VERSION 2.6)

find_package(PkgConfig)
if (_USE_JOBS)
	include_directories(${PROJECT_SOURCE_DIR}/jobs)
endif (_USE_JOBS)
add_library(c_dct_api STATIC dct_tools.c)
target_link_libraries(c_dct_api)
