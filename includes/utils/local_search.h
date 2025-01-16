#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "definitions.h"
#include "custom_cdt.h"
#include "circumcenter.h"
#include "midpoint.h"
#include "projection.h"
#include "obtuse.h"
#include "centroid.h"
#include "adjacent.h"
#include "random_steiner.h"
#include "ant_colony.h"
#include "convergence.h"

using namespace std;

void local_search_opt(CDT& cdt, Polygon_2& polygon, int max_iterations, vector<Point>& steiner, map<Point, int>& index, long double& convergence_value, bool random);

#endif // LOCAL_SEARCH_H