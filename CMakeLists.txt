project(ParallelGraph)


cmake_minimum_required(VERSION 2.8)

include(ExternalProject)


set(CMAKE_CXX_STANDARD 11)
link_libraries(pthread)   
include_directories(
  ${ParallelGraph_SOURCE_DIR}/src
  ${ParallelGraph_SOURCE_DIR}/deps/local/include)

ExternalProject_Add(eigen
  PREFIX ${ParallelGraph_SOURCE_DIR}/deps/eigen
  URL http://www.cjhang.com/targz/eigen_3.1.2.tar.bz2
  URL_MD5 e9c081360dde5e7dcb8eba3c8430fde2
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND cp -r Eigen unsupported <INSTALL_DIR>/
  INSTALL_DIR ${ParallelGraph_SOURCE_DIR}/deps/local/include)
add_definitions(-DHAS_EIGEN)

ExternalProject_Add(libbz2
  PREFIX ${ParallelGraph_SOURCE_DIR}/deps/libbz2
  URL ${ParallelGraph_SOURCE_DIR}/dist/bzip2-1.0.6.tar.gz
  URL_MD5 00b516f4704d4a7cb50a1d97e6e8e15b
  INSTALL_DIR ${ParallelGraph_SOURCE_DIR}/deps/local
  CONFIGURE_COMMAND ""
  PATCH_COMMAND patch -N -p0 -i ${ParallelGraph_SOURCE_DIR}/patches/libbz2_fpic.patch || true
  BUILD_IN_SOURCE 1
  BUILD_COMMAND make install PREFIX=<INSTALL_DIR>
  INSTALL_COMMAND "" )

ExternalProject_Add(boost
  PREFIX ${ParallelGraph_SOURCE_DIR}/deps/boost
  URL ${ParallelGraph_SOURCE_DIR}/dist/boost_1_53_0.tar.gz
  URL_MD5 57a9e2047c0f511c4dfcf00eb5eb2fbb
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND
  ./bootstrap.sh
  --with-libraries=filesystem
  --with-libraries=program_options
  --with-libraries=system
  --with-libraries=iostreams
  --with-libraries=date_time
  --with-libraries=random
  --with-libraries=context
  --prefix=<INSTALL_DIR>
  BUILD_COMMAND
  C_INCLUDE_PATH=${ParallelGraph_SOURCE_DIR}/deps/local/include
  CPLUS_INCLUDE_PATH=${ParallelGraph_SOURCE_DIR}/deps/local/include
  LIBRARY_PATH=${ParallelGraph_SOURCE_DIR}/deps/local/lib
  ./b2 install link=static variant=release threading=multi runtime-link=static
  INSTALL_COMMAND ""
  INSTALL_DIR ${ParallelGraph_SOURCE_DIR}/deps/local )
set(BOOST_ROOT ${ParallelGraph_SOURCE_DIR}/deps/local )
set(BOOST_LIBS_DIR ${ParallelGraph_SOURCE_DIR}/deps/local/lib)
message(STATUS ",,,," ${BOOST_LIBS_DIR})
set(Boost_LIBRARIES
  ${BOOST_LIBS_DIR}/libboost_filesystem.a
  ${BOOST_LIBS_DIR}/libboost_program_options.a
  ${BOOST_LIBS_DIR}/libboost_system.a
  ${BOOST_LIBS_DIR}/libboost_iostreams.a
  ${BOOST_LIBS_DIR}/libboost_context.a
  ${BOOST_LIBS_DIR}/libboost_date_time.a)
add_dependencies(boost libbz2)
message(STATUS "Boost libs: " ${Boost_LIBRARIES})  

ExternalProject_Add(libtcmalloc
  PREFIX ${ParallelGraph_SOURCE_DIR}/deps/tcmalloc
  URL ${ParallelGraph_SOURCE_DIR}/dist/gperftools-2.0.tar.gz
  URL_MD5 13f6e8961bc6a26749783137995786b6
  PATCH_COMMAND patch -N -p0 -i ${ParallelGraph_SOURCE_DIR}/patches/tcmalloc.patch || true
  CONFIGURE_COMMAND <SOURCE_DIR>/configure --enable-frame-pointers --prefix=<INSTALL_DIR> ${tcmalloc_shared}
  INSTALL_DIR ${ParallelGraph_SOURCE_DIR}/deps/local)
  #link_libraries(tcmalloc)
  set(TCMALLOC-FOUND 1)
add_definitions(-DHAS_TCMALLOC)

macro(requires_eigen NAME)
  add_dependencies(${NAME} eigen)
  add_dependencies(${NAME} libtcmalloc)
endmacro(requires_eigen)

macro(requires_boost NAME)
  add_dependencies(${NAME} boost)
endmacro(requires_boost)


subdirs(apps)
subdirs(tests)