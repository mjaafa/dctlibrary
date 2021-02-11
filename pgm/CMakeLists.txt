cmake_minimum_required (VERSION 2.6)

find_package(PkgConfig)
if (_USE_MATRIX_TOOLS)
	include_directories(${PROJECT_SOURCE_DIR}/matrix)
endif (_USE_MATRIX_TOOLS)

add_library(c_pgm_api STATIC pgm_reader.c)
target_link_libraries(c_pgm_api)
