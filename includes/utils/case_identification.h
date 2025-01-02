#ifndef CASE_IDENTIFICATION_H
#define CASE_IDENTIFICATION_H

#include <stack>				// DFS
#include <unordered_set>		// DFS
#include "definitions.h"
#include "adjacent.h" 			// is_convex_polygon()
#include "circumcenter.h"		// remove_faces_outside_boundary()


using namespace std;

int count_finite_faces(const CDT& triangulation);

bool polygon_is_convex_hull(const CDT& cdt, const Polygon_2& polygon);

string identify_case(const CDT& cdt, const Polygon_2& polygon, const int& constraint_count, const vector<pair<int, int>>& constraints);

#endif // CASE_IDENTIFICATION_H