#ifndef ADJACENT_H
#define ADJACENT_H

#include "definitions.h"

using namespace std;

// Function that checks if a vertex is part of a constraint
bool is_vertex_in_constraint(const CDT& cdt, Vertex_handle vertex);

bool is_convex(const vector<Point>& points);

bool is_convex_polygon(const Polygon_2& polygon);

Point calculate_polygon_center(const vector<Point>& points);

Point insert_adjacent(CDT& cdt, const Polygon_2& polygon);

Point steiner_adjacent_at_face(CDT& cdt, Face_handle& f, const Polygon_2& polygon);

#endif // ADJACENT_H