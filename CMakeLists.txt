cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME_STR midaco_solver)
project(${PROJECT_NAME_STR})

enable_language(C)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(3rd-party/global-optimization-test-problems)

add_library(${PROJECT_NAME_STR}_core ${CMAKE_CURRENT_SOURCE_DIR}/src/midaco_core.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/midaco_core.c)
target_include_directories(${PROJECT_NAME_STR}_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

#OMP library and sample
add_library(${PROJECT_NAME_STR}_omp ${CMAKE_CURRENT_SOURCE_DIR}/src/midaco_omp.hpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/midaco_omp.cpp)
target_link_libraries(${PROJECT_NAME_STR}_omp ${PROJECT_NAME_STR}_core ${OpenMP_CXX_LIBRARIES})
set_target_properties(${PROJECT_NAME_STR}_omp PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
target_include_directories(${PROJECT_NAME_STR}_omp PUBLIC ${GO_INTERFACE_INCLUDE_DIR}/)

add_executable(omp_example ${CMAKE_CURRENT_SOURCE_DIR}/samples/omp_sample.cpp)
target_link_libraries(omp_example ${PROJECT_NAME_STR}_omp grishagin_functions gkls_functions)
target_include_directories(omp_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/cmdline/
                                               ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/json
                                               ${CMAKE_CURRENT_SOURCE_DIR}/samples)

#MPI library and sample
add_library(${PROJECT_NAME_STR}_mpi ${CMAKE_CURRENT_SOURCE_DIR}/src/midaco_mpi.hpp
                                    ${CMAKE_CURRENT_SOURCE_DIR}/src/midaco_mpi.cpp)
target_link_libraries(${PROJECT_NAME_STR}_mpi ${PROJECT_NAME_STR}_core ${MPI_CXX_LIBRARIES} ${OpenMP_CXX_LIBRARIES})
set_target_properties(${PROJECT_NAME_STR}_mpi PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
target_include_directories(${PROJECT_NAME_STR}_mpi PUBLIC ${GO_INTERFACE_INCLUDE_DIR}/ ${MPI_CXX_INCLUDE_PATH})

add_executable(mpi_example ${CMAKE_CURRENT_SOURCE_DIR}/samples/mpi_sample.cpp)
target_link_libraries(mpi_example ${PROJECT_NAME_STR}_mpi grishagin_functions gkls_functions)
target_include_directories(mpi_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/cmdline/
                                               ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/json
                                               ${CMAKE_CURRENT_SOURCE_DIR}/samples)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(${PROJECT_NAME_STR}_core INTERFACE -Wall -march=native)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_definitions(/EHsc /openmp)
endif()
