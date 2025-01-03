#ifndef RANDOM_STEINER_H
#define RANDOM_STEINER_H

#include "definitions.h"

using namespace std;

Point insert_random(CDT& cdt, const Polygon_2& polygon);

Point steiner_random_at_face(Face_handle& face, const Polygon_2& polygon);

#endif // RANDOM_STEINER_H