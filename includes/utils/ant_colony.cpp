#include <cmath>
#include <random>
#include <map>

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
bool has_adjacent_obtuse_faces(CDT& cdt, Face_handle& f, Polygon_2& polygon) {

	// Add points of the current face to the obtuse polygon
	Polygon_2 obtuse_polygon_points;
	for (int i = 0; i < 3; ++i) {
		obtuse_polygon_points.push_back(f->vertex(i)->point());
	}
	// Check adjacent faces for obtuse angles
	for (int i = 0; i < 3; ++i) {
		Face_handle neighbor = f->neighbor(i);
		if (!cdt.is_infinite(neighbor)) {
			// Check if neighbor is inside boundary and has an obtuse angle
			bool neighbor_in_boundary = true;
			for (int j = 0; j < 3; ++j) {
				if (!polygon.has_on_bounded_side(neighbor->vertex(j)->point())) {
					neighbor_in_boundary = false;
					break;
				}
			}
			Point centroid = get_centroid(neighbor);
			if (is_point_outside_polygon(polygon, centroid)) {
                neighbor_in_boundary = false;
				break;
            }
			if (neighbor_in_boundary) {
				int neighbor_obtuse_index = find_obtuse_angle_index(neighbor);
				if (neighbor_obtuse_index != -1) {
					// Add points of the adjacent face to the obtuse polygon
					for (int j = 0; j < 3; ++j) {
						Point p = neighbor->vertex(j)->point();
						// if point is not in polygon, add it
						if (find(obtuse_polygon_points.begin(), obtuse_polygon_points.end(), p) == obtuse_polygon_points.end()) {
							obtuse_polygon_points.push_back(p);
						}
					}
				}
			}
		}
	}
    return is_convex_polygon(obtuse_polygon_points);
}

// Calculate heuristic value for every steiner option
vector<double> heuristic(CDT& cdt, Face_handle& face, Polygon_2& polygon) {
	vector<double> heuristic;
	double h[4];
	double r = radius_to_height_ratio(face);
	CDT cdt_copy = cdt;
	if(steiner_adjacent_at_face(cdt_copy, face, polygon) != Point(0.5, 0.5)) {
		cout << "Adjacent is 1" << endl;
		h[0] = 1.0;
	} else {
		h[0] = 0.0;
	}
	h[1] = max(0.0, (r - 1) / r);
	h[2] = r / (2 + r);
	h[3] = max(0.0, (3 - 2 * r) / 3.0);
	for(int i = 0; i < 4; i++) {
		heuristic.push_back(h[i]);
	}
	return heuristic;
}

Point improve_triangulation(CDT& cdt, Face_handle& face, Polygon_2& polygon, const int& xi, const int& psi, double pheromone[], int& to_return) {
	int i;
	double probability[4];
	vector<double> heuristic_values;

	heuristic_values = heuristic(cdt, face, polygon);
	// Now heuristic_values has heuristic values for every steiner option

	// Stores the total value of the sum (T_i^x * H_i^y) for i in steiner options in sum
	double sum = 0;
	for(i = 0; i < 4; i++) {
		sum += pow(pheromone[i], xi) * pow(heuristic_values[i], psi);
	}
	// Calculate probabilities
	for(i = 0; i < 4; i++) {
		probability[i] = double((pow(pheromone[i], xi) * pow(heuristic_values[i], psi))) / sum;
	}
	vector<double> cumulative;
	cumulative.push_back(probability[0]);
	for (i = 1; i < 4; i++) {
		cumulative.push_back(cumulative[i - 1] + probability[i]);
	}
	// Create a random device to seed the random number generator
	random_device rd;
	// Create a Mersenne Twister pseudo-random generator initialized with rd
	mt19937 gen(rd());
	// Create a uniform distribution for generating doubles between 0 and 1
	uniform_real_distribution<> distribution(0.0, 1.0);
	// random stores a value in [0,1]
	long double random = distribution(gen);
	for(i = 0; i < 4; i++) {
		if(random <= cumulative[i]) {
			break;
		}
	}
	Point to_insert;
	switch (i) {
		case 0:
			// insert_adjacent
			to_insert = steiner_adjacent_at_face(cdt, face, polygon);
			break;
		case 1:
			// insert projection
			cout << "Projection" << endl;
			to_insert = steiner_projection_at_face(face, polygon);
			cdt.insert(to_insert);		
			break;
		case 2:
			// insert circumcenter
			cout << "Circumcenter" << endl;
			to_insert = steiner_circumcenter_at_face(cdt, face, polygon);
			break;
		case 3:
			// insert midpoint
			cout << "Midpoint" << endl;
			to_insert = steiner_midpoint_at_face(face, polygon);
			cdt.insert(to_insert);
			break;
		default:
			throw invalid_argument("Invalid Steiner point option");
	}
	to_return = i;
	return to_insert;

}

double evaluate_triangulation(const CDT& cdt, Polygon_2& polygon, const int& steiner_count, const double& alpha, const double& beta) {
	int obtuse_count = count_obtuse_angles(cdt, polygon);
	return alpha * obtuse_count + beta * steiner_count;	// The lower the score, the better the triangulation
}

void update_pheromones(CDT& cdt, double pheromone[], const double& alpha, const double& beta, const double& lambda, map<Point, int>& good_ants,
						 Polygon_2& polygon, vector<Point_2>& steiner) {


	// Apply evaporation globally
	for (int i = 0; i < 4; i++) {
		pheromone[i] = (1 - lambda) * pheromone[i];
	}
	// Check for reinforcement
	for(auto it = good_ants.begin(); it != good_ants.end(); it++) {
		double delta_tau = 0.0;
		// check if current point was inserted in the triangulation
		// if it was inserted, it means that it improved the triangulation
		if(find(steiner.begin(), steiner.end(), it->first) != steiner.end()) {
			double evaluation = evaluate_triangulation(cdt, polygon, steiner.size(), alpha, beta);
			delta_tau = 1 / (1 + evaluation);
		}
		pheromone[it->second] += delta_tau;
	}
}

void ant_colony_optimization(CDT& cdt, Polygon_2& polygon, vector<Point_2>& steiner, const double& alpha, const double& beta,
							 const int& xi, const int& psi, const double& lambda, const int& kappa, const int& L, map<Point, int>& index) {

	
	double pheromone[4] = {1.0, 1.0, 1.0, 1.0};
	for(int cycle = 0; cycle < L; cycle++) {
		map<Point, int> good_ants;
		CDT cycle_best = cdt;
		double cycle_best_score = evaluate_triangulation(cycle_best, polygon, steiner.size(), alpha, beta);

		// face_handles vector will store faces inside given boundary
		vector<Face_handle> face_handles;
		for (Face_handle face : cdt.finite_face_handles()) {
			face_handles.push_back(face);
		}
		remove_faces_outside_boundary(face_handles, polygon);
		remove_non_obtuse_faces(face_handles);

		for (int ant = 0; ant < kappa; ant ++){
			// if there are no more obtuse faces,
			// break the loop
			if(face_handles.empty()) {
				break;
			}
			int to_return;
			CDT ant_triangulation = cycle_best;
			// pick an obtuse face for an ant
			Face_handle face = face_handles.back();
			face_handles.pop_back();
			// insert steiner point
			Point steiner_point = improve_triangulation(ant_triangulation, face, polygon, xi, psi, pheromone, to_return);

			// evaluate triangulation
			double score = evaluate_triangulation(ant_triangulation, polygon, steiner.size() + 1, alpha, beta);
			
			// if the steiner point improved the triangulation, we store the point in a temporary vector
			if(score <= cycle_best_score) {
				good_ants[steiner_point] = to_return;
			}

		}
		// save best triangulation
		for(auto it = good_ants.begin(); it != good_ants.end(); it++) {
			CDT temp = cycle_best;
			temp.insert(it->first);
			double score = evaluate_triangulation(temp, polygon, steiner.size() + 1, alpha, beta);
			if(score <= cycle_best_score) {
				cycle_best = temp;
				cycle_best_score = score;
				steiner.emplace_back(it->first);
				index[it->first] = index.size();
			}
		}
		cdt = cycle_best;
		update_pheromones(cdt, pheromone, alpha, beta, lambda, good_ants, polygon, steiner);
	}
}
