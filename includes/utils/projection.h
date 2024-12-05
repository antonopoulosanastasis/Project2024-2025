#ifndef PROJECTION_H
#define PROJECTION_H

#include "definitions.h"
#include "custom_cdt.h"

using namespace std;

Point insert_projection(CDT& cdt, const Polygon_2& polygon);

Point steiner_projection_at_face(Face_handle& face, const Polygon_2& polygon);

#endif // PROJECTION_H