cmake_minimum_required(VERSION 3.9)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
project(rabbits-and-foxes C)

#set(CMAKE_CXX_FLAGS_RELEASE "-fno-reorder-functions" "-fno-inline" "-no-pie")
#set(CMAKE_C_FLAGS_RELEASE "-fno-reorder-functions" "-fno-inline" "-no-pie")

#add_definitions(-DDEBUG)
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DENABLE_DEBUG_MACRO")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_DEBUG_MACRO")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
#add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")
#message("${CMAKE_SOURCE_DIR}")
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#add_definitions(-DNDEBUG)
#execute_process (
    #COMMAND bash -c "mpicc --showme:compile"
    #OUTPUT_VARIABLE mpi_compile
#)

#execute_process (
    #COMMAND bash -c "mpicc --showme:link"
    #OUTPUT_VARIABLE mpi_link
#)

##string(REPLACE "," " " mpi_link ${mpi_link})
#separate_arguments(mpi_link)
#separate_arguments(mpi_compile)

#message("${mpi_compile}")
#message("${mpi_link}")

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)
#list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/parallel.c)
#list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/serial_blocks.c)
add_executable(serial-rabbits-and-foxes ${SRC_FILES})
#target_link_libraries(serialdf m)
#target_link_libraries(serial-rabbits-and-foxes
  #PRIVATE
  #m
  #MPI::MPI_C)
#target_compile_options(serial-rabbits-and-foxes PRIVATE -O3)
target_compile_options(serial-rabbits-and-foxes PRIVATE -pg -g)
#target_link_options(serialdf PRIVATE -pg -g)

#target_compile_definitions(serialdf PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

#file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)
#list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/parallel.c)
#list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/serial.c)
#add_executable(serial-blocks-divisor-function ${SRC_FILES})
#target_link_libraries(serial-blocks-divisor-function m)

#file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)
##list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/serial_blocks.c)
#list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/serial.c)
#add_executable(paralleldf ${SRC_FILES})
#target_link_libraries(paralleldf
  #PRIVATE
  #m
  #MPI::MPI_C)
#target_compile_options(paralleldf PRIVATE -O3)
#target_compile_options(paralleldf PRIVATE "${mpi_compile}")
#target_link_options(paralleldf PRIVATE "${mpi_link}")

#target_compile_options(paralleldf PRIVATE ${MPI_C_COMPILE_FLAGS})
#target_link_options(paralleldf PRIVATE ${MPI_C_LINK_FLAGS})

