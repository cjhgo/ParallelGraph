project(ParallelGraph)

# subdirs(lbp)
# add_executable(main main.cpp)
add_executable(pagerank pagerank.cpp)
add_executable(demo demo.cpp)
requires_graphlab(demo)