#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include "definitions.h"
#include "custom_cdt.h"
#include "circumcenter.h"
#include "midpoint.h"
#include "projection.h"
#include "obtuse.h"
#include "centroid.h"

using namespace std;

void apply_best_sequence(CDT& cdt, Polygon_2& polygon,  const vector<string>& sequence, vector<Point_2>& steiner);

void try_combinations(CDT& cdt, Polygon_2& polygon, int max_depth, int current_depth, int& min_obtuse_angles, vector<string>& best_sequence, vector<string>& current_sequence, int& min_steiner_points);

void brute_force_steiner_insertion(CDT& cdt, int max_steiner_points, Polygon_2& polygon, vector<Point_2>& steiner);

#endif // BRUTE_FORCE_H