#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#pragma once

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <vector>

// Include the custom CDT header
#include "custom_cdt.h"

// Kernel and traits
typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Exact_predicates_tag Itag;

// Replace standard CDT with Custom CDT
typedef Custom_Constrained_Delaunay_triangulation_2<K, CGAL::Default, Itag> CDT;

// Update typedefs to use Custom CDT
typedef CDT::Point Point;
typedef CDT::Edge Edge;
typedef CDT::Face_handle Face_handle;
typedef CDT::Vertex_handle Vertex_handle;

// Polygon and geometry types
typedef CGAL::Polygon_2<K> Polygon_2;
typedef K::Point_2 Point_2;
typedef K::Line_2 Line;



#endif  // DEFINITIONS_H