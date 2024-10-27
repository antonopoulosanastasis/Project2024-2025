#include "projection.h"
#include "circumcenter.h"
#include "obtuse.h"

// Function to insert steiner point using
// the projection of the obtuse angle to the opposite side
Point insert_projection(CDT& cdt, const Polygon_2& polygon) {

	// face_handles vector will store faces inside given boundary
	std::vector<Face_handle> face_handles;
	for (Face_handle face : cdt.finite_face_handles()) {
		face_handles.push_back(face);
	}
	remove_faces_outside_boundary(face_handles, polygon);
	// Iterate over each face and check for obtuse angles
	for (Face_handle f : face_handles) {
		int obtuse_index = find_obtuse_angle_index(f); 
		if (obtuse_index != -1) {  // If there is an obtuse angle in the face
			// Get the vertex at the obtuse angle
			Point obtuse_vertex = f->vertex(obtuse_index)->point();

			// Get the two vertices opposite the obtuse angle
			Point opposite_p1 = f->vertex((obtuse_index + 1) % 3)->point();
			Point opposite_p2 = f->vertex((obtuse_index + 2) % 3)->point();

			// Create line from opposite points
			// and get the projection
			Line line(opposite_p1, opposite_p2);
			Point projection = line.projection(obtuse_vertex);

			cdt.insert(projection);

			return projection;
		}
	}
	return (Point)0;
}