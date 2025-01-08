add_library(Utils obtuse.cpp circumcenter.cpp projection.cpp midpoint.cpp brute_force.cpp local_search.cpp simulated_annealing.cpp centroid.cpp adjacent.cpp ant_colony.cpp case_identification.cpp random_steiner.cpp convergence.cpp)

target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})