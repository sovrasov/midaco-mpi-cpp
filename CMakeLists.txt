cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME_STR midaco_solver)
project(${PROJECT_NAME_STR})

enable_language(C)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)

add_library(${PROJECT_NAME_STR}_core ${CMAKE_CURRENT_SOURCE_DIR}/src/midaco_core.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/midaco_core.c)
set_property(TARGET ${PROJECT_NAME_STR}_core PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(${PROJECT_NAME_STR}_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(omp_example ${CMAKE_CURRENT_SOURCE_DIR}/samples/omp_sample.cpp)
target_link_libraries(omp_example ${PROJECT_NAME_STR}_core)
target_link_libraries(omp_example ${OpenMP_CXX_LIBRARIES})
set_target_properties(omp_example PROPERTIES COMPILE_FLAGS ${OpenMP_CXX_FLAGS})

add_executable(mpi_example ${CMAKE_CURRENT_SOURCE_DIR}/samples/mpi_sample.cpp)
target_link_libraries(mpi_example ${MPI_CXX_LIBRARIES} ${PROJECT_NAME_STR}_core)
target_include_directories(mpi_example PRIVATE ${MPI_CXX_INCLUDE_PATH})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(${PROJECT_NAME_STR}_core INTERFACE -Wall -march=native)
  target_compile_options(omp_example PRIVATE -fopenmp)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_definitions(/EHsc /openmp)
endif()
