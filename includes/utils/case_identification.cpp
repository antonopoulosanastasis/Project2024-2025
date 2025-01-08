#include "case_identification.h"

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

// Normalize edges to be undirected (smallest vertex first)
pair<int, int> normalize_edge(int u, int v) {
    return {min(u, v), max(u, v)};
}

// Check if an edge belongs to the constraint edges
bool is_constraint_edge(int u, int v, const set<pair<int, int>>& constraint_edges) {
	return constraint_edges.count(normalize_edge(u, v)) > 0;
}

bool has_cycle_with_constraints( const vector<pair<int, int>>& constraints, const vector<int>& boundary_vector) {
	map<int, vector<int>> adj_list;
	set<pair<int, int>> constraint_edges(constraints.begin(), constraints.end());
	set<pair<int, int>> boundary_edges;

	for (const auto& edge : constraints) {
		constraint_edges.insert(normalize_edge(edge.first, edge.second));
	}

	// Add boundary edges
	for (size_t i = 0; i < boundary_vector.size(); ++i) {
		int current = boundary_vector[i];
		int next = boundary_vector[(i + 1) % boundary_vector.size()];
		boundary_edges.insert({min(current, next), max(current, next)});
		adj_list[current].push_back(next);
		adj_list[next].push_back(current);
	}

	// Add constraint edges
	for (const auto& edge : constraints) {
		adj_list[edge.first].push_back(edge.second);
		adj_list[edge.second].push_back(edge.first);
	}

	set<int> visited;
	map<int, int> parent;

	for (const auto& node : adj_list) {
		int start = node.first;
		if (visited.count(start)) 
			continue;

		stack<pair<int, int>> s;
		s.push({start, -1});

		while (!s.empty()) {
			auto [current, par] = s.top();
			s.pop();

			if (visited.count(current)) 
				continue;

			visited.insert(current);
			parent[current] = par;

			for (int neighbor : adj_list[current]) {
				if (neighbor == par) 
					continue;

				if (visited.count(neighbor)) {
					if (neighbor != parent[current]) {
						set<pair<int, int>> cycle_edges;
						int temp = current;

						while (temp != -1 && temp != neighbor) {
							int par = parent[temp];
							if (par != -1) {
								cycle_edges.insert({min(temp, par), max(temp, par)});
							}
							temp = par;
						}
						cycle_edges.insert({min(neighbor, current), max(neighbor, current)});

						// Validate Cycle
						bool has_constraint_edge = false;

						for (const auto& edge : cycle_edges) {
							if (is_constraint_edge(edge.first, edge.second, constraint_edges)) {
								has_constraint_edge = true;
								break; // We only need one constraint edge to satisfy Case C
							}
						}

						if (has_constraint_edge) {
							return true;
						}
					}
				} else {
					s.push({neighbor, current});
				}
			}
		}
	}

	return false;
}

string identify_case(CDT& cdt, Polygon_2& polygon, int constraint_count, vector<pair<int, int>>& constraints, vector<int>& boundary_vector, vector<Point>& points) {
	// Convex boundary cases (A-C)
	if((polygon_is_convex_hull(cdt, polygon))) {
		// Case A: Convex boundary without constraints.
		if(constraint_count == 0) {
			return "A";
		}
		bool has_cycle = has_cycle_with_constraints(constraints, boundary_vector);
		if (has_cycle) {
			return "C";
		} else {
			return "B";
		}
	}
	// Non convex boundary cases (D, E)
	else {
		if((constraint_count == 0)) {
			bool all_edges_parallel_to_axes = true;

			// Check if all edges are parallel to the axes
			for (size_t i = 0; i < boundary_vector.size(); ++i) {
				int current_idx = boundary_vector[i];
				int next_idx = boundary_vector[(i + 1) % boundary_vector.size()]; // Wrap around to the first point
				Point current_point = points[current_idx];
				Point next_point = points[next_idx];

				// Check if the edge is horizontal or vertical
				if (current_point.x() != next_point.x() && current_point.y() != next_point.y()) {
					all_edges_parallel_to_axes = false;
					break;
				}
			}

			if (all_edges_parallel_to_axes) {
				return "D";
			}
		}
	}
	// if no other case was detected, it's case E
	return "E";
}