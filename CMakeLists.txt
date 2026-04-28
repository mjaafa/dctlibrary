cmake_minimum_required(VERSION 3.20)
project(dctlibrary LANGUAGES C)


set(dctlib_VERSION_MAJOR 2)
set(dctlib_VERSION_MINOR 2)

option(_USE_PGM "use pgm api" ON)
option(_USE_MATRIX_TOOLS "use matrix tools api" ON)
option(_USE_JOBS "use jobs api" OFF)
option(_USE_DCT "use dct api" ON)
option(BUILD_CUDA_MATRIX_TOOLS "build CUDA acceleration for matrix tools" OFF)
option(BUILD_CUDA_DCT "build CUDA acceleration for picture DCT" OFF)

# The CUDA DCT code depends on MatrixTools, and existing build scripts used
# BUILD_CUDA_DCT to enable both DCT and matrix multiply acceleration. Keep that
# behavior for compatibility while exposing MatrixTools as its own option.
if (BUILD_CUDA_DCT)
    set(BUILD_CUDA_MATRIX_TOOLS ON CACHE BOOL "build CUDA acceleration for matrix tools" FORCE)
endif()

if (_USE_JOBS)
    find_package(Threads REQUIRED)
endif()

if (_USE_DCT AND NOT _USE_MATRIX_TOOLS)
    message(FATAL_ERROR "You can't select [use dct api] because its dependency [use matrix tools api] was not met")
endif()

if ((BUILD_CUDA_MATRIX_TOOLS OR BUILD_CUDA_DCT) AND NOT _USE_MATRIX_TOOLS)
    message(FATAL_ERROR "CUDA MatrixTools requires _USE_MATRIX_TOOLS=ON")
endif()

if (BUILD_CUDA_MATRIX_TOOLS OR BUILD_CUDA_DCT)
    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)
endif()

configure_file(
    "${PROJECT_SOURCE_DIR}/dctlib.h.in"
    "${PROJECT_BINARY_DIR}/dctlib.h"
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:C>:/W3>)
else()
    add_compile_options($<$<COMPILE_LANGUAGE:C>:-O2;-Wall;-Wextra;-Wno-unused-parameter>)
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}
)

if (_USE_PGM)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/pgm)
endif()
if (_USE_MATRIX_TOOLS)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/matrix)
endif()
if (_USE_DCT)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dct)
endif()
if (_USE_JOBS)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/jobs)
endif()
if (BUILD_CUDA_MATRIX_TOOLS OR BUILD_CUDA_DCT)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cuda)
    add_subdirectory(cuda)
endif()

if (_USE_PGM)
    add_subdirectory(pgm)
endif()
if (_USE_MATRIX_TOOLS)
    add_subdirectory(matrix)
endif()
if (_USE_DCT)
    add_subdirectory(dct)
endif()
if (_USE_JOBS)
    add_subdirectory(jobs)
endif()

add_executable(dctlib src/main.c)

target_compile_definitions(dctlib PRIVATE
    $<$<BOOL:${BUILD_CUDA_MATRIX_TOOLS}>:USE_CUDA_MATRIX_TOOLS>
    $<$<BOOL:${BUILD_CUDA_DCT}>:USE_CUDA_DCT>)

if (_USE_PGM)
    target_link_libraries(dctlib PRIVATE c_pgm_api)
endif()
if (_USE_MATRIX_TOOLS)
    target_link_libraries(dctlib PRIVATE c_matrix_api)
endif()
if (_USE_DCT)
    target_link_libraries(dctlib PRIVATE c_dct_api)
endif()
if (_USE_JOBS)
    target_link_libraries(dctlib PRIVATE c_jobs_api Threads::Threads)
endif()
if (BUILD_CUDA_MATRIX_TOOLS)
    target_link_libraries(dctlib PRIVATE c_cuda_matrix_tools)
endif()
if (BUILD_CUDA_DCT)
    target_link_libraries(dctlib PRIVATE c_cuda_dct)
endif()
if (UNIX AND NOT APPLE)
    target_link_libraries(dctlib PRIVATE m)
endif()

configure_file(${PROJECT_SOURCE_DIR}/ressources/lenna.pgm ${PROJECT_BINARY_DIR}/lenna.pgm COPYONLY)
