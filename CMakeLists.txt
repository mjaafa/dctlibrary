cmake_minimum_required (VERSION 2.6)
project (dctlibrary)

####################################################
# NOTE : in this project isolation of compilers    #
#        is done explicitly even if cuda is enough #
#        aware of it. nvcc.                        #
####################################################

find_package(PkgConfig)
find_package(Threads REQUIRED)
#find_package(CUDA REQUIRED)

set (dctlib_VERSION_MAJOR 2)
set (dctlib_VERSION_MINOR 1)

set(CMAKE_VERBOSE_MAKEFILE ON)

option(_USE_PGM "use pgm api" ON)
option(_USE_MATRIX_TOOLS "use matrix tools api" ON)
option(_USE_JOBS "use jobs api" ON)
option(_USE_DCT "use dct api" ON)
option(BUILD_CUDA_DCT "use cuda_dct api" OFF)

# dependency 
if (_USE_DCT)
	if(NOT _USE_JOBS)
		MESSAGE(SEND_ERROR "You can't select [use dct api] because it's dependencies (use jobs api) were not met")
 	endif(NOT _USE_JOBS)
endif (_USE_DCT)

configure_file (
  "${PROJECT_SOURCE_DIR}/dctlib.h.in"
  "${PROJECT_BINARY_DIR}/dctlib.h"
  )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -O0 -g")

MESSAGE( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} " PROJECT_SOURCE_DIR " ${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include ${PC_PTHREAD_INCLUDE_DIRS} ${PROJECT_BINARY_DIR})

if (_USE_PGM)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pgm)
	add_subdirectory (pgm)
endif (_USE_PGM)
if (_USE_MATRIX_TOOLS)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/matrix)
	add_subdirectory (matrix)
endif (_USE_MATRIX_TOOLS)
if (_USE_DCT)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dct)
	add_subdirectory (dct)
endif (_USE_DCT)
if (_USE_JOBS)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/jobs)
	add_subdirectory (jobs)
endif (_USE_JOBS)
if (BUILD_CUDA_DCT)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cuda_dct)
	add_subdirectory (cuda_dct)
endif (BUILD_CUDA_DCT)


add_executable(dctlib src/main.c)

if (_USE_PGM)
 	target_link_libraries(dctlib c_pgm_api)
endif (_USE_PGM)
if (_USE_MATRIX_TOOLS)
 	target_link_libraries(dctlib c_matrix_api)
endif (_USE_MATRIX_TOOLS)
if (_USE_DCT)
 	target_link_libraries(dctlib c_dct_api)
endif (_USE_DCT)
if (_USE_JOBS)
 	target_link_libraries(dctlib c_jobs_api)
endif (_USE_JOBS)

target_link_libraries(dctlib ${PC_PTHREAD_LIBRARY_DIRS} -lpthread)

install (FILES ${PROJECT_SOURCE_DIR}/ressources/lenna.pgm DESTINATION ${PROJECT_BINARY_DIR})
