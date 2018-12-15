project(ParallelGraph)


cmake_minimum_required(VERSION 2.8)

include(ExternalProject)


set(CMAKE_CXX_STANDARD 11)



set(POWERGRAPH_DIR /home/chen/workspace/PowerGraph)

message(STATUS ":::" ${POWERGRAPH_DIR})

include_directories(
  ${ParallelGraph_SOURCE_DIR}/src
  ${POWERGRAPH_DIR}/deps/local/include)

set(BOOST_ROOT ${POWERGRAPH_DIR}/deps/local )
set(BOOST_LIBS_DIR ${POWERGRAPH_DIR}/deps/local/lib)
message(STATUS ",,,," ${BOOST_LIBS_DIR})
set(Boost_LIBRARIES
  ${BOOST_LIBS_DIR}/libboost_filesystem.a
  ${BOOST_LIBS_DIR}/libboost_program_options.a
  ${BOOST_LIBS_DIR}/libboost_system.a
  ${BOOST_LIBS_DIR}/libboost_iostreams.a
  ${BOOST_LIBS_DIR}/libboost_context.a
  ${BOOST_LIBS_DIR}/libboost_date_time.a)

link_libraries(pthread)     
link_directories(${POWERGRAPH_DIR}/deps/local/lib)
subdirs(apps tests )

find_package(MPI) 
if(MPI_FOUND)
  message(STATUS "MPI Found: " ${MPI_LIBRARY} ${MPI_EXTRA_LIBRARY})
  add_definitions(-DHAS_MPI)
  include_directories(${MPI_INCLUDE_PATH})
else(MPI_FOUND)
  message(STATUS "MPI Not Found! Distributed Executables will not be compiled")
  set(MPI_LIBRARY "")
  set(MPI_EXTRA_LIBRARY "")
endif(MPI_FOUND)

# Core graphlab requirements
macro(requires_graphlab NAME)
  target_link_libraries(${NAME}
    ${Boost_LIBRARIES}
    z
    event 
    event_pthreads
    zookeeper_mt
    graphlab)

#Equivalent to
# link_libraries(/home/chen/workspace/PowerGraph/debug/src/graphlab/libgraphlab.a
#   -lpthread 
#   /home/chen/workspace/PowerGraph/deps/local/lib/libzookeeper_mt.a 
#   /home/chen/workspace/PowerGraph/deps/local/lib/libevent.a
#   /home/chen/workspace/PowerGraph/deps/local/lib/libevent_pthreads.a
# )    
  if(MPI_FOUND)
    target_link_libraries(${NAME} pthread ${MPI_LIBRARY} ${MPI_EXTRA_LIBRARY})
  endif(MPI_FOUND)    
endmacro(requires_graphlab)
link_libraries(${MPI_LIBRARY} ${MPI_EXTRA_LIBRARY})
