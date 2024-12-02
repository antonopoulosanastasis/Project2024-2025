# Created by the script cgal_create_CMakeLists
# This is the CMake script for compiling a set of CGAL applications.

cmake_minimum_required(VERSION 3.1...3.23)

project( const )


# CGAL and its components
find_package( CGAL QUIET COMPONENTS Qt5 )

if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  

endif()


# Boost and its components
find_package( Boost REQUIRED COMPONENTS system filesystem )

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()


# include for local directory
include_directories(${CMAKE_SOURCE_DIR}/includes/utils)
add_subdirectory(includes/utils)
# include for local package
find_package(Qt5 REQUIRED COMPONENTS Widgets)

# Creating entries for target: const
# ############################

add_executable( opt_triangulation opt_triangulation.cpp )

add_to_cached_list( CGAL_EXECUTABLE_TARGETS opt_triangulation )

# Link the executable to CGAL and third-party libraries
target_link_libraries(opt_triangulation PRIVATE CGAL::CGAL Utils Qt5::Widgets )

if (CGAL_Qt5_FOUND)
  add_definitions(-DCGAL_USE_BASIC_VIEWER)
  target_link_libraries(opt_triangulation PRIVATE CGAL::CGAL_Qt5)
endif()