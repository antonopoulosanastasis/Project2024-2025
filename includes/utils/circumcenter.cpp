#include "circumcenter.h"
#include "obtuse.h"

// Function to get the circumcenter of a face (triangle)
Point get_circumcenter(Face_handle face) {
	// Get the vertices of the triangle
	Point p1 = face->vertex(0)->point();
	Point p2 = face->vertex(1)->point();
	Point p3 = face->vertex(2)->point();

	// CGAL provides a built-in function to compute the circumcenter
	return CGAL::circumcenter(p1, p2, p3);
}

// Function to get the centroid of a face (triangle)
Point get_centroid(Face_handle face) {
	// Get the vertices of the triangle
	Point p1 = face->vertex(0)->point();
	Point p2 = face->vertex(1)->point();
	Point p3 = face->vertex(2)->point();

	// CGAL provides a built-in function to compute the centroid
	return CGAL::centroid(p1, p2, p3);
}

// Function to check if the point is outside the polygon
bool is_point_outside_polygon(const Polygon_2& polygon, const Point_2& point) {
    CGAL::Bounded_side result = CGAL::bounded_side_2(polygon.vertices_begin(), polygon.vertices_end(), point, K());

    // Return true if the point is outside the polygon
    return (result == CGAL::ON_UNBOUNDED_SIDE);
}

// Function to remove faces outside boundary from a face vector
void remove_faces_outside_boundary(std::vector<CDT::Face_handle>& face_vector, const Polygon_2& boundary) {
	std::vector<CDT::Face_handle>::iterator it = face_vector.begin();

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
	std::vector<Face_handle> face_handles;
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

            // Check if the circumcenter is inside or on the boundary of the polygon
            CGAL::Bounded_side circumcenter_location = CGAL::bounded_side_2(polygon.vertices_begin(), polygon.vertices_end(), circumcenter, K());

            if (circumcenter_location != CGAL::ON_UNBOUNDED_SIDE) {
                // If the circumcenter is inside or on the boundary, insert it
                cdt.insert(circumcenter);
				return circumcenter;
                //cout << "Inserted circumcenter at (" << circumcenter.x() << ", " << circumcenter.y() << ") to break up obtuse triangle.\n";
            } else {
                // Otherwise, compute and insert the centroid
                Point centroid = get_centroid(f);
                cdt.insert(centroid);
				return centroid;
                //cout << "Inserted centroid at (" << centroid.x() << ", " << centroid.y() << ") to break up obtuse triangle.\n";
            }
        }
    }
	return Point();
}
