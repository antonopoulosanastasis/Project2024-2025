add_library(Utils obtuse.cpp circumcenter.cpp projection.cpp midpoint.cpp brute_force.cpp)

target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})