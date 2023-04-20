cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(MAP-UOT LANGUAGES CXX CUDA)

# cmake_host_system_information(RESULT var QUERY HAS_SSE2)
# message(${var})

# cmake for MAP-UOT_CPU
list(APPEND flags_C "-O3" "-mavx2")

add_library(MAP-UOT_TestCPU STATIC MAP-UOT_Testprint_0.cpp MAP-UOT_Testtypical_0.cpp MAP-UOT_TestMap.cpp)

target_compile_options(MAP-UOT_TestCPU PRIVATE ${flags_C})

add_executable(MAP-UOT_CPU MAP-UOT_CPU.cpp)

target_compile_options(MAP-UOT_CPU PRIVATE ${flags_C})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(MAP-UOT_CPU PUBLIC MAP-UOT_TestCPU Threads::Threads)

# cmake for MAP-UOT_MPI
find_package(MPI REQUIRED)

add_executable(MAP-UOT_MPI MAP-UOT_MPI.cpp MAP-UOT_Testprint_0.cpp MAP-UOT_TestMPItypical.cpp MAP-UOT_TestMPIMap.cpp)

target_compile_options(MAP-UOT_MPI PRIVATE ${flags_C})

target_link_libraries(MAP-UOT_MPI PUBLIC MPI::MPI_CXX)

# cmake for MAP-UOT_GPU
list(APPEND flags_G "-O3")

add_library(MAP-UOT_TestGPU STATIC MAP-UOT_Testprint_1.cpp MAP-UOT_Testtypical_1.cpp MAP-UOT_TestGPUMap.cu)

target_compile_options(MAP-UOT_TestGPU PRIVATE ${flags_G})

add_executable(MAP-UOT_GPU MAP-UOT_GPU.cu)

target_compile_options(MAP-UOT_GPU PRIVATE ${flags_G})

target_link_libraries(MAP-UOT_GPU PUBLIC MAP-UOT_TestGPU)
