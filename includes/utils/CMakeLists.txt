add_library(Utils obtuse.cpp circumcenter.cpp projection.cpp midpoint.cpp brute_force.cpp local_search.cpp simulated_annealing.cpp centroid.cpp adjacent.cpp)

target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})