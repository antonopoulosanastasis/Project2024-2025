#ifndef ADJACENT_H
#define ADJACENT_H

#include "definitions.h"

using namespace std;

bool is_convex(const vector<Point>& points);

Point calculate_polygon_center(const vector<Point>& points);

Point insert_adjacent(CDT& cdt, const Polygon_2& polygon);

#endif // ADJACENT_H