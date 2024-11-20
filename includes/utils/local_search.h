#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "definitions.h"
#include "custom_cdt.h"
#include "circumcenter.h"
#include "midpoint.h"
#include "projection.h"
#include "obtuse.h"

using namespace std;

void local_search_optimization(CDT& cdt, Polygon_2& polygon, int max_iterations, vector<Point>& steiner);

#endif // LOCAL_SEARCH_H