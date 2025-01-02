#include "case_identification.h"

// Function to build the adjacency list
// This graph approach will be used to search with DFS for a cycle (case C)
map<int, vector<int>> build_adjacency_list(const vector< pair<int, int>>& constraints) {
	map<int, vector<int>> adjacency_list;

	for (const auto& constraint : constraints) {
		// Add both directions for the undirected graph
		adjacency_list[constraint.first].push_back(constraint.second);
		adjacency_list[constraint.second].push_back(constraint.first);
	}

	return adjacency_list;
}

// Function to detect a cycle in the graph using DFS with a stack
bool detect_cycle(const map<int, vector<int>>& adjacency_list, int start_node) {
	unordered_set<int> visited;		// set to keep all visited nodes in
	stack<pair<int, int>> stack; 	// Pair of (current node, parent node)

	// Initialize the stack with the starting node
	stack.push({start_node, -1});

	while (!stack.empty()) {
		auto [current, parent] = stack.top();
		stack.pop();

		// If the node is already visited, visited.count(current node) 
		// should return true, which means we've detected a cycle
		if (visited.count(current)) {
			return true;
		}

		visited.insert(current);

		// Traverse neighbors
		for (int neighbor : adjacency_list.at(current)) {
			// Skip the edge to the parent node (avoid trivial cycle detection)
			if (neighbor != parent) {
				stack.push({neighbor, current});
			}
		}
	}

	return false;
}

// Function that checks for cycles in a graph using stack (DFS)
bool has_cycle(const map<int, vector<int>>& adjacency_list) {
	unordered_set<int> visited;

	// Check each connected component
	for (const auto& pair : adjacency_list) {
		int node = pair.first;
		if (!visited.count(node)) {
			if (detect_cycle(adjacency_list, node)) {
				return true;
			}
		}
	}
	return false;
}

int count_finite_faces(const CDT& triangulation) {
	int face_count = 0;
	for (auto face = triangulation.finite_faces_begin(); face != triangulation.finite_faces_end(); face++) {
		face_count++;
	}
	return face_count;
}

bool polygon_is_convex_hull(const CDT& cdt, const Polygon_2& polygon) {
	int initial_face_count = count_finite_faces(cdt);

	vector<Face_handle> face_handles;
	for (Face_handle face : cdt.finite_face_handles()) {
		face_handles.push_back(face);
	}
	remove_faces_outside_boundary(face_handles, polygon);
	int final_count = face_handles.size();

	return initial_face_count == final_count;
}

string identify_case(const CDT& cdt, const Polygon_2& polygon, const int& constraint_count, const vector<pair<int, int>>& constraints) {
	// Convex boundary cases (A-C)
	if(is_convex_polygon(polygon)) {
		// Case A: Convex boundary without constraints.
		if( (polygon_is_convex_hull(cdt, polygon)) && (constraint_count == 0) ) {
			return "A";
		}
		// Further check for cases B and C
		for (const auto& constraint : constraints) {
			Point p1 = polygon[constraint.first];
			Point p2 = polygon[constraint.second];
			if (!polygon.bounded_side(p1) || !polygon.bounded_side(p2)) {
				// Case C: Convex boundary with inside constraints.
				return "C";
			}
		}
		// Case B: Convex boundary with open constraints.
		return "B";
	}
}