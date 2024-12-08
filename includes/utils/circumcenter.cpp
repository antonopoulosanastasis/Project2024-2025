#include "circumcenter.h"
#include "obtuse.h"
#include "centroid.h"

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

Point steiner_circumcenter_at_face(Face_handle& face, const Polygon_2& polygon) {
	Point centroid = get_centroid(face);
	if (is_point_outside_polygon(polygon, centroid)) {
		return Point();
	}
	int obtuse_index = find_obtuse_angle_index(face);
	if (obtuse_index != -1) {  // If there is an obtuse angle in the face
		// Compute the circumcenter of the triangle
		Point circumcenter = get_circumcenter(face);

		if (!is_point_outside_polygon(polygon, circumcenter)) {
			// If the circumcenter is inside or on the boundary, return it
			return circumcenter;
        } else {
			// Otherwise, compute and return the centroid of the CURRENT face
			Point centroid = get_centroid(face);
			return centroid;
		}
	}
	return Point(0.5, 0.5);
}
