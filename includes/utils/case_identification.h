#ifndef CASE_IDENTIFICATION_H
#define CASE_IDENTIFICATION_H

#include <stack>				// DFS
#include <unordered_set>		// DFS
#include <map>					// DFS
#include <vector>
#include <string>
#include <utility>
#include <set>
#include <queue>
#include <iostream>
#include "definitions.h"
#include "adjacent.h" 			// is_convex_polygon()
#include "circumcenter.h"		// remove_faces_outside_boundary()

using namespace std;

int count_finite_faces(const CDT& triangulation);

bool polygon_is_convex_hull(const CDT& cdt, const Polygon_2& polygon);

void debug_graph(const map<int, vector<int>>& adj_list);

// Check if an edge belongs to the constraint edges
bool is_constraint_edge(int u, int v, const set<pair<int, int>>& constraint_edges);

bool has_cycle_with_constraints( const vector<pair<int, int>>& constraints, const vector<int>& boundary_vector);

string identify_case(CDT& cdt, Polygon_2& polygon, int constraint_count, vector<pair<int, int>>& constraints, vector<int>& boundary_vector, vector<Point>& points);

#endif // CASE_IDENTIFICATION_H