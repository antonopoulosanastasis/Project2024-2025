#ifndef OBTUSE_H
#define OBTUSE_H

#include "definitions.h"

using namespace std;

// Function to count obtuse angles of a triangulation 
int count_obtuse_angles(CDT cdt, Polygon_2& polygon);

// Function to deserialize constraints from JSON
bool is_obtuse_angle(const Point& p0, const Point& p1, const Point& p2);

// Function to check if a face contains any obtuse angles
bool has_obtuse_angle(Face_handle f);

// Function to check if a triangulation is obtuse
bool is_obtuse_triangulation(CDT cdt);

// Helper function to find the index of the obtuse angle in a face
int find_obtuse_angle_index(Face_handle f);

void remove_non_obtuse_faces(vector<Face_handle>& faces);

#endif // OBTUSE_H