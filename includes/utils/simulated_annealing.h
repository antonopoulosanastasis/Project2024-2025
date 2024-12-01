#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include <random>
#include <cmath>

#include "definitions.h"
#include "custom_cdt.h"
#include "circumcenter.h"
#include "midpoint.h"
#include "projection.h"
#include "obtuse.h"
#include "centroid.h"

using namespace std;

void simulated_annealing_optimization(CDT& cdt, Polygon_2& polygon, vector<Point>& steiner);

// Function to compute energy
double compute_energy(CDT& cdt, const Polygon_2& polygon, int steiner_count);

// Function to pick steiner point method based on given number <option>
Point choose_steiner_point(CDT& cdt, const Polygon_2& polygon, int option);

#endif // SIMULATED_ANNEALING_H