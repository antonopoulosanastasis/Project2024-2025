#include "midpoint.h"
#include "circumcenter.h"
#include "obtuse.h"

// Function to insert a point on the midpoint of the edge opposite to the obtuse angle
Point insert_midpoint(CDT& cdt, const Polygon_2& boundary) {
	// face_handles vector will store faces inside given boundary
	std::vector<Face_handle> face_handles;
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
            //cout << "Inserted point at (" << midpoint.x() << ", " << midpoint.y() << ") to break up obtuse triangle.\n";
            return midpoint;
        }
    }
	return (Point)0;
}