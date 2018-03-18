project(ParallelGraph)


cmake_minimum_required(VERSION 2.8)

include(ExternalProject)


set(CMAKE_CXX_STANDARD 11)

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

macro(requires_eigen NAME)
  add_dependencies(${NAME} eigen)
endmacro(requires_eigen)


subdirs(apps)
subdirs(tests)