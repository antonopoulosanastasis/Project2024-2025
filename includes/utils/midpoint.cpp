#include "midpoint.h"
#include "circumcenter.h"
#include "obtuse.h"

// Function to insert a point on the midpoint of the edge opposite to the obtuse angle
Point insert_midpoint(CDT& cdt, const Polygon_2& boundary) {
	// face_handles vector will store faces inside given boundary
	vector<Face_handle> face_handles;
	for (Face_handle face : cdt.finite_face_handles()) {
		face_handles.push_back(face);
	}
	remove_faces_outside_boundary(face_handles, boundary);

	// Iterate over each face and check for obtuse angles
	for (Face_handle f : face_handles) {
		int obtuse_index = find_obtuse_angle_index(f);
		if (obtuse_index != -1) {  // If there is an obtuse angle in the face
			// Get the two vertices opposite the obtuse angle
			Point opposite_p1 = f->vertex((obtuse_index + 1) % 3)->point();
			Point opposite_p2 = f->vertex((obtuse_index + 2) % 3)->point();

			// Compute the midpoint of the edge opposite the obtuse angle
			Point midpoint = CGAL::midpoint(opposite_p1, opposite_p2);

			// Insert the midpoint into the triangulation
			cdt.insert(midpoint);
			return midpoint;
		}
	}
	return Point();
}

Point steiner_midpoint_at_face(Face_handle& face, const Polygon_2& polygon) {
	Point centroid = get_centroid(face);
	if (is_point_outside_polygon(polygon, centroid)) {
		return Point();
	}
	int obtuse_index = find_obtuse_angle_index(face);
	if (obtuse_index != -1) {  // If there is an obtuse angle in the face
		// Get the two vertices opposite the obtuse angle
		Point opposite_p1 = face->vertex((obtuse_index + 1) % 3)->point();
		Point opposite_p2 = face->vertex((obtuse_index + 2) % 3)->point();

		// Compute the midpoint of the edge opposite the obtuse angle
		Point midpoint = CGAL::midpoint(opposite_p1, opposite_p2);

		return midpoint;
	}
	return Point(0.5, 0.5);
}