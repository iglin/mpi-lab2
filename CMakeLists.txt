cmake_minimum_required(VERSION 3.7)
project(mpi_lab2)

set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
find_package(MPI REQUIRED)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

set(SOURCE_FILES main.cpp)
add_executable(mpi_lab2 ${SOURCE_FILES})

target_link_libraries(mpi_lab2 ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
    set_target_properties(hello PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(mpi_lab2 PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()