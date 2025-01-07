#include "case_identification.h"

// Function to combine constraints and get them in a certain format
// in order to search for cycles (cases B & C)
vector<pair<pair<int, int>, bool>> combine_constraints(const vector<pair<int, int>>& inner_constraints, vector<int>& boundary_vector) {
	vector<pair<pair<int, int>, bool>> formatted_constraints;
	// Add boundary constraints from the polygon
	for (int i = 0; i < boundary_vector.size(); i++) {
		int node1 = boundary_vector[i];									// Get the actual index from region_boundary
		int node2 = boundary_vector[(i + 1) % boundary_vector.size()];	// Wrap around to the first node
		formatted_constraints.push_back({{node1, node2}, false}); 		// Boundary = false
	}
	// Add inner constraints
	for (const auto& constraint : inner_constraints) {
		formatted_constraints.push_back({constraint, true});			// Inner = true
	}
	return formatted_constraints;
}

// Function to build the adjacency list
// This graph approach will be used to search with DFS for a cycle (case C)
map<int, vector<pair<int, bool>>> build_adjacency_list(const vector<pair<pair<int, int>, bool>>& constraints) {
	map<int, vector<pair<int, bool>>> adjacency_list;

	for (const auto& constraint : constraints) {
		int node1 = constraint.first.first;   // First node in the constraint
		int node2 = constraint.first.second;  // Second node in the constraint
		bool is_inner = constraint.second;    // True if it's an inner constraint

		// Add the edge to the adjacency list for both directions
		adjacency_list[node1].emplace_back(node2, is_inner);
		adjacency_list[node2].emplace_back(node1, is_inner);
	}

	return adjacency_list;
}

// Function to detect inner or mixed cycles for a component
bool detect_inner_or_mixed_cycle_for_component(const map<int, vector<pair<int, bool>>>& adjacency_list, int& start_node, unordered_set<int>& visited) {
	stack<pair<int, int>> stack;		// Pair of (current node, parent node)
	unordered_set<int> local_visited;	// Tracks nodes locally for cycles
	bool has_inner_constraint = false;	// Tracks if cycle has an inner constraint
	bool has_cycle = false;				// Tracks if any cycle is detected

	stack.push(make_pair(start_node, -1));

	while (!stack.empty()) {
		pair<int, int> current_pair = stack.top();
		int current = current_pair.first;
		int parent = current_pair.second;
		stack.pop();

		// If the node is already in the local_visited set, a cycle is detected
		if (local_visited.count(current)) {
			has_cycle = true;
			// Check for inner constraints in the cycle
			for (size_t i = 0; i < adjacency_list.at(current).size(); ++i) {
				int neighbor = adjacency_list.at(current)[i].first;
				bool is_inner = adjacency_list.at(current)[i].second;
				// If neighbour is not parent node and is already visited
				// we have to check if the edge connecting current and neighbor is an inner constraint
				if (neighbor != parent && local_visited.count(neighbor)) {
					if (is_inner) {
						has_inner_constraint = true; 	// Mark if cycle includes inner constraints
					}
				}
			}
			continue; // Continue searching for other cycles
		}

		// Mark the current node as visited
		local_visited.insert(current);
		visited.insert(current); 		// make sure current node won't be visited in subsequent DFS traversals

		// Push neighbors into the stack
		for (size_t i = 0; i < adjacency_list.at(current).size(); ++i) {
			int neighbor = adjacency_list.at(current)[i].first;
			if (neighbor != parent) {
				stack.push(make_pair(neighbor, current));
			}
		}
	}

	// has to return true if there IS a cycle that has at least one inner constraint
	return has_cycle && has_inner_constraint;
}

// Function to check for inner or mixed cycles in the entire graph
bool has_inner_or_mixed_cycles(const map<int, vector<pair<int, bool>>>& adjacency_list) {
	unordered_set<int> visited;

	for (auto it = adjacency_list.begin(); it != adjacency_list.end(); ++it) {
		int node = it->first;
		if (!visited.count(node)) {
			if (detect_inner_or_mixed_cycle_for_component(adjacency_list, node, visited)) {
				return true; 	// Return immediately if any valid cycle is found
			}
		}
	}
	return false; // No cycles with inner constraints found
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
        if (visited.count(start)) continue;

        stack<pair<int, int>> s;
        s.push({start, -1});

        while (!s.empty()) {
            auto [current, par] = s.top();
            s.pop();

            if (visited.count(current)) continue;
            visited.insert(current);
            parent[current] = par;

            for (int neighbor : adj_list[current]) {
                if (neighbor == par) continue;

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