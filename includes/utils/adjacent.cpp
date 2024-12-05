#include <map>
#include "adjacent.h"
#include "obtuse.h"
#include "circumcenter.h"

// Utility to check if a polygon is convex
bool is_convex(const vector<Point>& points) {
	if (points.size() < 3) return false;
	bool is_ccw = CGAL::orientation(points[0], points[1], points[2]) == CGAL::COUNTERCLOCKWISE;
	for (size_t i = 1; i < points.size(); ++i) {
		size_t next = (i + 1) % points.size();
		size_t prev = (i - 1 + points.size()) % points.size();
		if (CGAL::orientation(points[prev], points[i], points[next]) != (is_ccw ? CGAL::COUNTERCLOCKWISE : CGAL::CLOCKWISE)) {
			return false;
		}
	}
	return true;
}

// Utility to calculate the center of a polygon
Point calculate_polygon_center(const vector<Point>& points) {
	K::FT x = 0, y = 0;
	for (const auto& point : points) {
		x += point.x();
		y += point.y();
	}
	auto size = static_cast<K::FT>(points.size());
	return Point(x / size, y / size);
}


// Main function: Insert adjacent obtuse center
Point insert_adjacent(CDT& cdt, const Polygon_2& polygon) {
	// Collect all faces inside the polygon boundary
	vector<Face_handle> face_handles;
	for (Face_handle face : cdt.finite_face_handles()) {
		face_handles.push_back(face);
	}
	remove_faces_outside_boundary(face_handles, polygon);

	for (Face_handle f : face_handles) {
		int obtuse_index = find_obtuse_angle_index(f);
		if (obtuse_index != -1) { // Current face has an obtuse angle
			vector<Point> obtuse_polygon_points;
			vector<pair<Point, Point>> polygon_edges;

			// Add points of the current face to the obtuse polygon
			for (int i = 0; i < 3; ++i) {
				obtuse_polygon_points.push_back(f->vertex(i)->point());
			}

			// Check adjacent faces for obtuse angles
			for (int i = 0; i < 3; ++i) {
				Face_handle neighbor = f->neighbor(i);
				if (!cdt.is_infinite(neighbor)) {
					// Check if neighbor is inside boundary and has an obtuse angle
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
								if (find(obtuse_polygon_points.begin(), obtuse_polygon_points.end(), p) == obtuse_polygon_points.end()) {
									obtuse_polygon_points.push_back(p);
								}
							}
							// Collect edges between the face and the neighbor
							for (int j = 0; j < 3; ++j) {
								Point p1 = neighbor->vertex(j)->point();
								Point p2 = neighbor->vertex((j + 1) % 3)->point();
								polygon_edges.emplace_back(p1, p2);
							}
						}
					}
				}
			}

			// Check if the formed polygon is convex
			if (is_convex(obtuse_polygon_points)) {
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
				for (const auto& point : obtuse_polygon_points) {
					cdt.remove(point);
				}
				// Step 4: Calculate the center of the polygon (centroid of points)
				Point center = calculate_polygon_center(obtuse_polygon_points);

				// Step 5: Insert the polygon center into the CDT
				cdt.insert(center);

				// Step 6: Re-insert the original polygon points
				for (const auto& point : obtuse_polygon_points) {
					cdt.insert(point);
				}

				// Step 7: Unmark the external edges
				for (const auto& edge : external_edges) {
					cdt.remove_constraint(edge.first, edge.second);
				}
				// Return the inserted point
				return center;
			}
		}
	}
	// If no valid point was inserted, return default-constructed point
	return Point();
}