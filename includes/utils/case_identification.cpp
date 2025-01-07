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

void debug_graph(const map<int, vector<int>>& adj_list) {
	cout << "Adjacency List:" << endl;
	for (const auto& [node, neighbors] : adj_list) {
		cout << node << ": ";
		for (int neighbor : neighbors) {
			cout << neighbor << " ";
		}
		cout << endl;
	}
}

// Normalize edges to be undirected (smallest vertex first)
auto normalize_edge = [](int u, int v) -> pair<int, int> {
	return {min(u, v), max(u, v)};
};

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

	// Debug adjacency list
	debug_graph(adj_list);

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
						cout << "Detected Cycle:" << endl;
						set<pair<int, int>> cycle_edges;
						int temp = current;

						while (temp != -1 && temp != neighbor) {
							int par = parent[temp];
							if (par != -1) {
								cycle_edges.insert({min(temp, par), max(temp, par)});
								cout << "Edge: (" << par << ", " << temp << ")" << endl;
							}
							temp = par;
						}
						cycle_edges.insert({min(neighbor, current), max(neighbor, current)});
						cout << "Edge: (" << current << ", " << neighbor << ")" << endl;

						// Validate Cycle
						bool has_constraint_edge = false;

						for (const auto& edge : cycle_edges) {
							if (is_constraint_edge(edge.first, edge.second, constraint_edges)) {
								has_constraint_edge = true;
								break; // We only need one constraint edge to satisfy Case C
							}
						}

						cout << "Cycle Validation:" << endl;
						cout << "Cycle Edges: ";
						for (const auto& edge : cycle_edges) {
							cout << "(" << edge.first << ", " << edge.second << ") ";
						}
						cout << endl;
						cout << "Has Constraint Edge: " << has_constraint_edge << endl;

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
		cout << "Boundary is convex hull" << endl;
		// Case A: Convex boundary without constraints.
		if(constraint_count == 0) {
			cout << "case A" << endl;
			return "A";
		}
		bool has_cycle = has_cycle_with_constraints(constraints, boundary_vector);
		if (has_cycle) {
			cout << "case C" << endl;
			return "C";
		} else {
			cout << "case B" << endl;
			return "B";
		}
	}
	// Non convex boundary cases (D, E)
	else {
		cout << "Boundary is not convex hull" << endl;
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
				cout << "Case D" << endl;
				return "D";
			}
		}
	}
	// if no other case was detected, it's case E
	cout << "Case E" << endl;
	return "E";
}