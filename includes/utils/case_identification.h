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

vector<pair<pair<int, int>, bool>> combine_constraints(const vector<pair<int, int>>& inner_constraints, vector<int>& boundary_vector);

// Function to build the adjacency list
// This graph approach will be used to search with DFS for a cycle (case C)
map<int, vector<pair<int, bool>>> build_adjacency_list(const vector<pair<pair<int, int>, bool>>& constraints);

// Function to detect a cycle in the graph using DFS with a stack
bool detect_inner_or_mixed_cycle_for_component(const map<int, vector<pair<int, bool>>>& adjacency_list, int& start_node, unordered_set<int>& visited);

// Function that checks for cycles in a graph using stack (DFS)
bool has_inner_or_mixed_cycles(const map<int, vector<pair<int, bool>>>& adjacency_list);

int count_finite_faces(const CDT& triangulation);

bool polygon_is_convex_hull(const CDT& cdt, const Polygon_2& polygon);

bool has_cycle_with_constraints( const vector<pair<int, int>>& constraints, const vector<int>& boundary_vector);

string identify_case(CDT& cdt, Polygon_2& polygon, int constraint_count, vector<pair<int, int>>& constraints, vector<int>& boundary_vector, vector<Point>& points);

#endif // CASE_IDENTIFICATION_H