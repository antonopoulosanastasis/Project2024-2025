#include "centroid.h"
#include "obtuse.h"
#include "circumcenter.h"	// for remove_faces_outside_boundary()

// Function to get the centroid of a face (triangle)
Point get_centroid(Face_handle face) {
	// Get the vertices of the triangle
	Point p1 = face->vertex(0)->point();
	Point p2 = face->vertex(1)->point();
	Point p3 = face->vertex(2)->point();

	// CGAL provides a built-in function to compute the centroid
	return CGAL::centroid(p1, p2, p3);
}

Point insert_centroid(CDT& cdt, const Polygon_2& polygon) {
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

Point steiner_centroid_at_face(Face_handle& face, const Polygon_2& polygon) {
	Point centroid = get_centroid(face);
	if (is_point_outside_polygon(polygon, centroid)) {
		return Point();
	}
	int obtuse_index = find_obtuse_angle_index(face);
	if (obtuse_index != -1) {  // If there is an obtuse angle in the face
		Point centroid = get_centroid(face);
		return centroid;
	}
	return Point(0.5, 0.5);
}