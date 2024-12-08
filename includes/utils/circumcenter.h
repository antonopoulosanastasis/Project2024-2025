#ifndef CIRCUMCENTER_H
#define CIRCUMCENTER_H

#include "definitions.h"

using namespace std;

Point get_circumcenter(Face_handle face);

Point get_centroid(Face_handle face);

bool is_point_outside_polygon(const Polygon_2& polygon, const Point_2& point);

void remove_faces_outside_boundary(vector<Face_handle>& face_vector, const Polygon_2& boundary);

Point insert_circumcenter(CDT& cdt, const Polygon_2& polygon);

Point steiner_circumcenter_at_face(CDT& cdt, Face_handle& face, const Polygon_2& polygon);

#endif // CIRCUMCENTER_H