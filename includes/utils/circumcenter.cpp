#include "circumcenter.h"
#include "obtuse.h"
#include "centroid.h"
#include "adjacent.h"

// Function to get the circumcenter of a face (triangle)
Point get_circumcenter(Face_handle face) {
	// Get the vertices of the triangle
	Point p1 = face->vertex(0)->point();
	Point p2 = face->vertex(1)->point();
	Point p3 = face->vertex(2)->point();

	// CGAL provides a built-in function to compute the circumcenter
	return CGAL::circumcenter(p1, p2, p3);
}

// Function to check if the point is outside the polygon
bool is_point_outside_polygon(const Polygon_2& polygon, const Point_2& point) {
	CGAL::Bounded_side result = CGAL::bounded_side_2(polygon.vertices_begin(), polygon.vertices_end(), point, K());

	// Return true if the point is outside the polygon
	return (result == CGAL::ON_UNBOUNDED_SIDE);
}

// Function to remove faces outside boundary from a face vector
void remove_faces_outside_boundary(vector<Face_handle>& face_vector, const Polygon_2& boundary) {
	vector<Face_handle>::iterator it = face_vector.begin();

	// Iterate through the vector and remove faces based on the condition
	while (it != face_vector.end()) {
		if (is_point_outside_polygon(boundary, get_centroid(*it)) ) {
			it = face_vector.erase(it); // Remove face and get new iterator
		} else {
			++it; // Move to the next face
		}
	}
}

// Function to insert the circumcenter or centroid of the face with an obtuse angle
Point insert_circumcenter(CDT& cdt, const Polygon_2& polygon) {
	// face_handles vector will store faces inside given boundary
	vector<Face_handle> face_handles;
	for (Face_handle face : cdt.finite_face_handles()) {
		face_handles.push_back(face);
	}
	remove_faces_outside_boundary(face_handles, polygon);
	// Iterate over each face and check for obtuse angles
	for (Face_handle f : face_handles) {
		int obtuse_index = find_obtuse_angle_index(f);
		if (obtuse_index != -1) {  // If there is an obtuse angle in the face
		// Compute the circumcenter of the triangle
			Point circumcenter = get_circumcenter(f);

			if (!is_point_outside_polygon(polygon, circumcenter)) {
				// If the circumcenter is inside or on the boundary, insert it
				cdt.insert(circumcenter);
				return circumcenter;
            } else {
				// Otherwise, compute and insert the centroid of the CURRENT face
				// calling insert_centroid() here does not guarantee that THIS face will be picked
				Point centroid = get_centroid(f);
				cdt.insert(centroid);
				return centroid;
			}
		}
	}
	return Point();
}

Point steiner_circumcenter_at_face(CDT& cdt, Face_handle& face, const Polygon_2& polygon) {
	Point centroid = get_centroid(face);
	if (is_point_outside_polygon(polygon, centroid)) {
		return Point();
	}
	int obtuse_index = find_obtuse_angle_index(face);
	if (obtuse_index != -1) {  // If there is an obtuse angle in the face
		// Compute the circumcenter of the triangle
		Point circumcenter = get_circumcenter(face);
		if(!is_point_outside_polygon(polygon, circumcenter)) {
			Point obtuse_point = face->vertex(obtuse_index)->point();

			vector<Point> obtuse_polygon_points;
			vector<pair<Point, Point>> polygon_edges;
			// Add points of the current face to the obtuse polygon
			for (int i = 0; i < 3; ++i) {
				obtuse_polygon_points.push_back(face->vertex(i)->point());
			}
			Face_handle neighbor = face->neighbor(obtuse_index);
			bool neighbor_in_boundary = true;
			for (int j = 0; j < 3; ++j) {
				if (!polygon.has_on_bounded_side(neighbor->vertex(j)->point())) {
					neighbor_in_boundary = false;
					break;
				}
			}
			if (neighbor_in_boundary) {
				int neighbor_obtuse_index = find_obtuse_angle_index(neighbor);
				if (neighbor_obtuse_index != -1) {
					// Add points of the adjacent face to the obtuse polygon
					for (int j = 0; j < 3; ++j) {
						Point p = neighbor->vertex(j)->point();
						// if point is not in polygon, add it
						if (find(obtuse_polygon_points.begin(), obtuse_polygon_points.end(), p) == obtuse_polygon_points.end()) {
							obtuse_polygon_points.push_back(p);
						}
					}
					// Collect edges between the face and the neighbor
					for (int j = 0; j < 3; ++j) {
						Point p1 = neighbor->vertex(j)->point();
						Point p2 = neighbor->vertex((j + 1) % 3)->point();
						pair<Point, Point> edge = make_pair(p1, p2);
						polygon_edges.emplace_back(edge);
					}
				}
			}
			map<pair<Point, Point>, int> edge_count;
			// Count occurrences of each edge in the polygon_edges
			for (const auto& edge : polygon_edges) {
				// with this check, we make sure that the edge is normalized
				// (A,B) and (B,A) are treated as the same edge
				auto normalized_edge = edge.first < edge.second
					? make_pair(edge.first, edge.second)
					: make_pair(edge.second, edge.first);
				edge_count[normalized_edge]++;
			}
			// Step 1: Identify the outer edges of the polygon
			vector<pair<Point, Point>> external_edges;
			for (const auto& [edge, count] : edge_count) {
				if (count == 1) { // Edge is not shared
					external_edges.push_back(edge);
				}
			}
			// Step 2: Mark the external edges as constraints
			for (const auto& edge : external_edges) {
				cdt.insert_constraint(edge.first, edge.second);
			}
			// Step 3: Remove the polygon points from the CDT
			for (auto& point : obtuse_polygon_points) {
				// Find the vertex handle corresponding to the point
				for (auto v = cdt.finite_vertices_begin(); v != cdt.finite_vertices_end(); ++v) {
					if (v->point() == point) {
						// if the point is part of a constraint, skip the removal
						if(is_vertex_in_constraint(cdt, v)) {
							break;
						} else {
							// Remove the vertex from the CDT
							cdt.remove(v);
							break;
						}
					}
				}
			}
			cdt.insert_no_flip(circumcenter);
			cdt.insert_constraint(obtuse_point, circumcenter);

			// Step 6: Re-insert the original polygon points
			for (const auto& point : obtuse_polygon_points) {
				cdt.insert_no_flip(point);
			}

			// Step 7: Unmark the external edges
			for (const auto& edge : external_edges) {
				auto fh = cdt.locate(CGAL::midpoint(edge.first, edge.second)); // Locate a face near the edge
				for (int i = 0; i < 3; ++i) {
					auto edge_vertices = make_pair(fh->vertex(i)->point(), fh->vertex((i + 1) % 3)->point());
					// Normalize the edge vertices
					auto normalized_edge = edge_vertices.first < edge_vertices.second
						? make_pair(edge_vertices.first, edge_vertices.second)
						: make_pair(edge_vertices.second, edge_vertices.first);

					// If the edge matches the current external edge, remove the constraint
					if (normalized_edge == edge) {
						cdt.remove_constraint(fh, i);
						break;
					}
				}
			}
			Face_handle fh = cdt.locate(CGAL::midpoint(circumcenter, obtuse_point));
			auto edge = circumcenter < obtuse_point
				? make_pair(circumcenter, obtuse_point)
				: make_pair(obtuse_point, circumcenter);
			for(int i = 0; i < 3; i++) {
				auto edge_vertices = make_pair(fh->vertex(i)->point(), fh->vertex((i + 1) % 3)->point());
				auto normalized_edge = edge_vertices.first < edge_vertices.second
						? make_pair(edge_vertices.first, edge_vertices.second)
						: make_pair(edge_vertices.second, edge_vertices.first);
				if(normalized_edge == edge) {
					cdt.remove_constraint(fh, i);
					break;
				}
			}
			return circumcenter;
			
		} else {
			Point centroid = get_centroid(face);
			cdt.insert(centroid);
			return centroid;
		}

	}
	return Point(0.5, 0.5);
}
