#include <cmath>

#include "circumcenter.h"
#include "ant_colony.h"
#include "adjacent.h"
#include "obtuse.h"
#include "simulated_annealing.h"	// for Energy computation

// Calculates distance between two points
double compute_distance(const Point& p1, const Point& p2) {
	return sqrt(CGAL::to_double(CGAL::squared_distance(p1, p2)));
}

// Function to calculate the circumradius
double circumradius(Face_handle& face) {
	// Get the vertices of the triangle
	Point_2 A = face->vertex(0)->point();
	Point_2 B = face->vertex(1)->point();
	Point_2 C = face->vertex(2)->point();

	// Compute the side lengths, converting CGAL::FT to double
	double a = compute_distance(B, C);
	double b = compute_distance(A, C);
	double c = compute_distance(A, B);

	// Calculcate area of the face
	double area = abs(CGAL::to_double(CGAL::area(A, B, C)));

	// Circumradius formula: R = (a * b * c) / (4 * area)
	return (a * b * c) / (4.0 * area);
}

// Function to calculate the height from longest side
double longest_side_height(Face_handle& face) {
	// Array that stores the lengths of each side of the face
	double lengths[3];
	Point_2 A = face->vertex(0)->point();
	Point_2 B = face->vertex(1)->point();
	Point_2 C = face->vertex(2)->point();
	lengths[0] = compute_distance(A, B);
	lengths[1] = compute_distance(B, C);
	lengths[2] = compute_distance(C, A);

	// Find longest side and its length
	int longest_index = 0;
	for (int i = 1; i < 3; ++i) {
		if (lengths[i] > lengths[longest_index]) {
			longest_index = i;
		}
	}
	// Calculate area of the face
	double area = abs(CGAL::to_double(CGAL::area(A, B, C)));

	// area = (height * longest_base) / 2 => height = (2 * area) / longest_base
	return (2.0 * area) / lengths[longest_index];
}

// Function to calculate the ratio r = circumradius / triangle height from longest side
double radius_to_height_ratio(Face_handle& face) {
	return circumradius(face) / longest_side_height(face);
}

// Check if adjacent method should be prioritized
bool has_adjacent_obtuse_faces(Face_handle& face, const Polygon_2& polygon) {
	for (int i = 0; i < 3; i++) {
		Face_handle neighbor = face->neighbor(i);
        // Check if neighbor is inside boundary and has an obtuse angle
        bool neighbor_in_boundary = true;
        for (int j = 0; j < 3; ++j) {
            if (!polygon.has_on_bounded_side(neighbor->vertex(j)->point())) {
                neighbor_in_boundary = false;
                break;
            }
		}
        if (neighbor_in_boundary) {
            int neighbor_obtuse_index = find_obtuse_angle_index(neighbor);
            if (neighbor_obtuse_index != -1) {
				return true;
			}
		}
	}
	return false;
}

// Calculate heuristic value for every steiner option
double* heuristic(Face_handle& face, const Polygon_2& polygon) {
	double heuristic[4];
	double r = radius_to_height_ratio(face);
	if(has_adjacent_obtuse_faces(face, polygon)) {
		heuristic[0] = 1.0;
	} else {
		heuristic[0] = 0.0;
	}
	heuristic[1] = max(0.0, (r - 1) / r);
	heuristic[2] = r / (2 + r);
	heuristic[3] = max(0.0, (3 - 2 * r) / 3.0);
	return heuristic;
}

// Calculate probabilities for every steiner option, and pick the one with the highest probability
void improve_triangulation(CDT& cdt, Face_handle& face, const Polygon_2& polygon, const double& xi, const double& psi, double pheromone[]) {
	int i;
	double probability[4];
	double heuristic_values[4];
	heuristic_values = heuristic(face, polygon);
	// Now heuristic_values has heuristic values for every steiner option

	// Stores the total value of the sum (T_i^x * H_i^y) for i in steiner options in sum
	int sum = 0;
	for(i = 0; i < 4; i++) {
		sum += pow(pheromone[i], xi) * pow(heuristic_values[i], psi);
	}
	// Calculate probabilities
	for(i = 0; i < 4; i++) {
		probability[i] = (pow(pheromone[i], xi) * pow(heuristic_values[i], psi)) / sum;
	}
	// Random number generator
	default_random_engine generator;
	// Creates a Mersenne Twister random number generator, seeded with the value from rd().
	// here, we use rd() as a seed to make sure the values differ every time it runs
	mt19937 gen(rd());
	// uniform distribution for a random number in [0,1]
	uniform_real_distribution<double> distribution(0.0, 1.0);
	// random stores a value in [0,1]
	double random = distribution(generator);
	double total_probability = 0.0;
	for(i = 0; i < 4; i++) {
		total_probability += probability[i];
		if(random <= total_probability) {
			break;
		}
	}
	Point to_insert;
	switch (i) {
		case 0:
			// insert_adjacent
			insert_adjacent(cdt, polygon);
			return;
		case 1:
			// insert projection
			to_insert = steiner_projection_at_face(face, polygon);
			cdt.insert(to_insert);
			break;
		case 2:
			// insert circumcenter
			to_insert = steiner_circumcenter_at_face(face, polygon);
			break;
		case 3:
			// insert midpoint
			to_insert = steiner_midpoint_at_face(face, polygon);
			break;
		default:
			throw invalid_argument("Invalid Steiner point option");
	}
	cdt.insert(to_insert);
}

void ant_colony_optimization(CDT& cdt, const Polygon_2& polygon, vector<Point_2>& steiner, const double& alpha, const double& beta,
							 const double& xi, const double& psi, const double& lambda, const int& kappa, const int& L) {

}
