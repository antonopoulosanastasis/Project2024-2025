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
double circumradius(Face_handle face) {
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

double longest_side_height(Face_handle face) {
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

double radius_to_height_ratio(Face_handle face) {
	return circumradius(face) / longest_side_height(face);
}

bool has_adjacent_obtuse_faces(Face_handle face, const Polygon_2& polygon) {
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

double heuristic(Face_handle& face, const Polygon_2& polygon) {
	if(has_adjacent_obtuse_faces(face, polygon)) {
		// h_{mean adjacent} = 1
		return 1.0;
	}
	double r = radius_to_height_ratio(face);
	double h;
	if(r < 1.0) {
		h = max(0.0, (3 - 2 * r) / 3.0);
	} else if (r < 2.0) {
		h = r / (2 + r);
	} else {
		h = max(0.0, (r - 1) / r);
	}
	return h;
}



void ant_colony_optimization(CDT& cdt, const Polygon_2& polygon, vector<Point_2>& steiner, double alpha, double beta, double xi, double psi, double lambda, int kappa, int L) {

}
