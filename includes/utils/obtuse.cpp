#include "custom_cdt.h"
#include "obtuse.h"
#include "definitions.h"

// Helper function to check if the angle at p1 between p0 and p2 is obtuse
bool is_obtuse_angle(const Point& p0, const Point& p1, const Point& p2) {
    // Vector p0 -> p1 and p2 -> p1
    K::Vector_2 v1 = p0 - p1;
    K::Vector_2 v2 = p2 - p1;
    
    // Compute the dot product
    auto dot_product = v1 * v2;
    
    // The angle is obtuse if the dot product is negative
    return dot_product < 0;
}

// Function to check if a face contains any obtuse angles
bool has_obtuse_angle(Face_handle f) {
    // Get the three vertices of the face
    Point p0 = f->vertex(0)->point();
    Point p1 = f->vertex(1)->point();
    Point p2 = f->vertex(2)->point();
    
    // Check all three angles
    if (is_obtuse_angle(p1, p0, p2)) {
        return true;  // Angle at p0 is obtuse
    }
    if (is_obtuse_angle(p0, p1, p2)) {
        return true;  // Angle at p1 is obtuse
    }
    if (is_obtuse_angle(p0, p2, p1)) {
        return true;  // Angle at p2 is obtuse
    }

    return false;  // No obtuse angles found
}

// Function to check if a triangulation is obtuse
bool is_obtuse_triangulation(CDT cdt) {
    for (Face_handle f : cdt.finite_face_handles()) {
        if (has_obtuse_angle(f)) {
            return true; // Found a face with an obtuse angle, exit early
        }
    }
    return false; // No obtuse angles found in any faces
}

// Helper function to find the index of the obtuse angle in a face
int find_obtuse_angle_index(Face_handle f) {
    // Get the vertices of the face
    Point p0 = f->vertex(0)->point();
    Point p1 = f->vertex(1)->point();
    Point p2 = f->vertex(2)->point();
    
    // Check which angle is obtuse
    if (is_obtuse_angle(p1, p0, p2)) {
        return 0;  // Angle at vertex 0 is obtuse
    }
    if (is_obtuse_angle(p0, p1, p2)) {
        return 1;  // Angle at vertex 1 is obtuse
    }
    if (is_obtuse_angle(p0, p2, p1)) {
        return 2;  // Angle at vertex 2 is obtuse
    }
    return -1;  // No obtuse angle
}