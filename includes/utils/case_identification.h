#ifndef CASE_IDENTIFICATION_H
#define CASE_IDENTIFICATION_H

#include <stack>				// DFS
#include <unordered_set>		// DFS
#include <map>					// DFS
#include "definitions.h"
#include "adjacent.h" 			// is_convex_polygon()
#include "circumcenter.h"		// remove_faces_outside_boundary()

using namespace std;

// Function to build the adjacency list
// This graph approach will be used to search with DFS for a cycle (case C)
map<int, vector<int>> build_adjacency_list(const vector< pair<int, int>>& constraints);

// Function to detect a cycle in the graph using DFS with a stack
bool detect_cycle(const map<int, vector<int>>& adjacency_list, int start_node);

// Function that checks for cycles in a graph using stack (DFS)
bool has_cycle(const map<int, vector<int>>& adjacency_list);

int count_finite_faces(const CDT& triangulation);

bool polygon_is_convex_hull(const CDT& cdt, const Polygon_2& polygon);

string identify_case(const CDT& cdt, const Polygon_2& polygon, const int& constraint_count, const vector<pair<int, int>>& constraints);

#endif // CASE_IDENTIFICATION_H