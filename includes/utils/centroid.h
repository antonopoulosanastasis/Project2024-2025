#ifndef CENTROID_H
#define CENTROID_H

#include "definitions.h"

using namespace std;

Point get_centroid(Face_handle face);

Point insert_centroid(CDT& cdt, const Polygon_2& polygon);

Point steiner_centroid_at_face(Face_handle& face, const Polygon_2& polygon);

#endif // CENTROID_H