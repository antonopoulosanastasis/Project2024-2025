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
#include "adjacent.h"
#include "convergence.h"
#include "random_steiner.h"

using namespace std;

void simulated_annealing_opt(CDT& cdt, Polygon_2& polygon, vector<Point>& steiner, double alpha, double beta, int L, map<Point, int>& index, long double& convergence, bool random);

// Function to compute energy
double compute_energy(CDT& cdt, const Polygon_2& polygon, int steiner_count, double alpha, double beta);

// Function to pick steiner point method based on given number <option>
Point choose_steiner(CDT& cdt, const Polygon_2& polygon, int option, Face_handle& face);

#endif // SIMULATED_ANNEALING_H