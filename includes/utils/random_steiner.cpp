#include "random_steiner.h"
#include "centroid.h"
#include "obtuse.h"
#include "circumcenter.h"	// for remove_faces_outside_boundary()
#include <cmath> // For mathematical functions like sqrt
#include <cstdlib> // For random number generation

Point insert_random(CDT& cdt, const Polygon_2& polygon) {
	// face_handles vector will store faces inside given boundary
	vector<Face_handle> face_handles;
	for (Face_handle face : cdt.finite_face_handles()) {
		face_handles.push_back(face);
	}
	remove_faces_outside_boundary(face_handles, polygon);
	// Iterate over each face and check for obtuse angles
	for (Face_handle f : face_handles) {
		int obtuse_index = find_obtuse_angle_index(f);
		if (obtuse_index != -1) {	// If there is an obtuse angle in the face
			// Calculate centroid
			Point centroid = get_centroid(f);

			// insert in triangulation and return the point
			cdt.insert(centroid);
			return centroid;
		}
	}
	return Point();
}

Point steiner_random_at_face(Face_handle& face, const Polygon_2& polygon) {
    Point centroid = get_centroid(face);

    if (is_point_outside_polygon(polygon, centroid)) {
        return Point();
    }

    // Function to calculate the distance between a point and a line segment
    auto point_to_segment_distance = [](const Point& p, const Point& a, const Point& b) {
        double px = to_double(b.x() - a.x());
        double py = to_double(b.y() - a.y());

        double norm = px * px + py * py;
        double u = to_double(((p.x() - a.x()) * px + (p.y() - a.y()) * py) / norm);
        u = std::max(0.0, std::min(1.0, u));
        Point closest(a.x() + u * px, a.y() + u * py);
        double dx = to_double(p.x() - closest.x());
        double dy = to_double(p.y() - closest.y());
        return std::sqrt(dx * dx + dy * dy);
    };

    // Find the minimum distance from the centroid to the sides of the triangle
    std::vector<Point> vertices;
    for (int i = 0; i < 3; ++i) {  // Triangles have 3 vertices
        vertices.push_back(face->vertex(i)->point());
    }

    double min_distance = std::numeric_limits<double>::max();
    for (size_t i = 0; i < vertices.size(); ++i) {
        Point a = vertices[i];
        Point b = vertices[(i + 1) % vertices.size()];
        double distance = point_to_segment_distance(centroid, a, b);
        min_distance = std::min(min_distance, distance);
    }

    // Generate random offsets within the range [-min_distance, min_distance]
    double offset_x = ((double)rand() / RAND_MAX) * 2 * min_distance - min_distance;
    double offset_y = ((double)rand() / RAND_MAX) * 2 * min_distance - min_distance;

    Point offset_centroid(centroid.x() + offset_x, centroid.y() + offset_y);

    // Ensure the offset centroid remains inside the polygon
    if (!is_point_outside_polygon(polygon, offset_centroid)) {
        return offset_centroid;
    }

    // If the random offset goes outside, return the original centroid
    return centroid;
}