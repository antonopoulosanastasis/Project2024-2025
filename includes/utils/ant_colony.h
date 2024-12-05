#ifndef ANT_COLONY_H
#define ANT_COLONY_H

#include "definitions.h"

using namespace std;

// Function to calculate circumradius of a face
double circumradius(Face_handle& face);

// Function to calculate the height from longest side
double longest_side_height(Face_handle& face);

// Function to calculate the ratio r = circumradius / triangle height from longest side
double radius_to_height_ratio(Face_handle& face);

// Check if adjacent method should be prioritized
bool has_adjacent_obtuse_faces(Face_handle& face, const Polygon_2& polygon);

// Calculate heuristic value for every steiner option
vector<double>  heuristic(Face_handle& face, const Polygon_2& polygon);

// Calculate probabilities for every steiner option, and pick the one with the highest probability
void improve_triangulation(CDT& cdt, Face_handle& face, const Polygon_2& polygon, const double& xi, const double& psi, double pheromone[]);

void ant_colony_optimization(CDT& cdt, const Polygon_2& polygon, vector<Point_2>& steiner, const double& alpha, const double& beta,
							 const double& xi, const double& psi, const double& lambda, const int& kappa, const int& L);

#endif // ANT_COLONY_H