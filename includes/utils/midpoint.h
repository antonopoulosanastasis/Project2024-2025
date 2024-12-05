#ifndef MIDPOINT_H
#define MIDPOINT_H

#include "definitions.h"
#include "custom_cdt.h"

using namespace std;

Point insert_midpoint(CDT& cdt, const Polygon_2& polygon);

Point steiner_midpoint_at_face(Face_handle& face, const Polygon_2& polygon);

#endif // MIDPOINT_H