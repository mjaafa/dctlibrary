cmake_minimum_required(VERSION 3.20)

if (BUILD_CUDA_MATRIX_TOOLS)
    add_library(c_cuda_matrix_tools STATIC cuda_matrix_tools.cu)
    set_target_properties(c_cuda_matrix_tools PROPERTIES
        CUDA_STANDARD 17
        CUDA_STANDARD_REQUIRED YES
        CUDA_SEPARABLE_COMPILATION OFF
        POSITION_INDEPENDENT_CODE ON)
    target_compile_options(c_cuda_matrix_tools PRIVATE
        $<$<COMPILE_LANGUAGE:CUDA>:-O3;--use_fast_math>)

    target_include_directories(c_cuda_matrix_tools PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR})

    target_compile_definitions(c_cuda_matrix_tools PUBLIC USE_CUDA_MATRIX_TOOLS)
    target_link_libraries(c_cuda_matrix_tools PUBLIC CUDA::cudart)
endif()

if (BUILD_CUDA_DCT)
    add_library(c_cuda_dct STATIC cuda_dct.cu)
    set_target_properties(c_cuda_dct PROPERTIES
        CUDA_STANDARD 17
        CUDA_STANDARD_REQUIRED YES
        CUDA_SEPARABLE_COMPILATION OFF
        POSITION_INDEPENDENT_CODE ON)
    target_compile_options(c_cuda_dct PRIVATE
        $<$<COMPILE_LANGUAGE:CUDA>:-O3;--use_fast_math>)

    target_include_directories(c_cuda_dct PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../dct
        ${CMAKE_CURRENT_SOURCE_DIR}/../matrix
        ${PROJECT_BINARY_DIR})

    target_compile_definitions(c_cuda_dct PUBLIC USE_CUDA_DCT)
    target_link_libraries(c_cuda_dct PUBLIC CUDA::cudart)
endif()

# Single translation units; avoid RDC/device-link on MSVC builds.
