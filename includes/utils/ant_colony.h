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
bool has_adjacent_obtuse_faces(CDT& cdt, Face_handle& face, Polygon_2& polygon);

// Calculate heuristic value for every steiner option
vector<double>  heuristic(CDT& cdt, Face_handle& face, const Polygon_2& polygon);

Point improve_triangulation(CDT& cdt, Face_handle& face, Polygon_2& polygon, const int& xi, const int& psi, double pheromone[], int& to_return);

double evaluate_triangulation(const CDT& cdt, Polygon_2& polygon, const int& steiner_count, const double& alpha, const double& beta);

void update_pheromones(CDT& cdt, double pheromone[], const double& alpha, const double& beta, const double& lambda, map<Point, int>& good_ants,
						 Polygon_2& polygon, vector<Point_2>& steiner);

void ant_colony_optimization(CDT& cdt, Polygon_2& polygon, vector<Point_2>& steiner, const double& alpha, const double& beta,
							 const int& xi, const int& psi, const double& lambda, const int& kappa, const int& L, map<Point, int>& index, long double& convergence_value, bool random);

#endif // ANT_COLONY_H