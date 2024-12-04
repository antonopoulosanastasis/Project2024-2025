#ifndef ANT_COLONY_H
#define ANT_COLONY_H

#include "definitions.h"

using namespace std;

double circumradius(Face_handle face);

double longest_side_height(Face_handle face);

double radius_to_height_ratio(Face_handle face);

bool has_adjacent_obtuse_faces(Face_handle face, const Polygon_2& polygon);

double heuristic(Face_handle face, const Polygon_2& polygon);

void ant_colony_optimization(CDT& cdt, const Polygon_2& polygon, vector<Point_2>& steiner, double alpha, double beta, double xi, double psi, double lambda, int kappa, int L);

#endif // ANT_COLONY_H