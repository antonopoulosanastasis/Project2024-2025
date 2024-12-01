#include "simulated_annealing.h"

// Constants needed for Energy calculation
// Energy = ALPHA * <number of obtuse angles> + BETA * <steiner point count>
#define ALPHA 3.0
#define BETA  0.5
#define L     5000		// number of iterations in simulated annealing algorithm

double compute_energy(CDT& cdt, Polygon_2& polygon, int steiner_count) {
	return ALPHA * count_obtuse_angles(cdt, polygon) + BETA * steiner_count;
}

Point choose_steiner_point(CDT& cdt, const Polygon_2& polygon, int option) {
	switch (option) {
		case 1:
			return insert_projection(cdt, polygon);
		case 2:
			return insert_midpoint(cdt, polygon);
		case 3:
			return insert_circumcenter(cdt, polygon);
		case 4:
			return insert_centroid(cdt, polygon);
		default:
			throw invalid_argument("Invalid Steiner point option");
	}
}

void simulated_annealing_optimization(CDT& cdt, Polygon_2& polygon, vector<Point>& steiner) {
	double energy = compute_energy(cdt, polygon, 0);
	double temperature = 1.0;

	// Random number generator
	default_random_engine generator;
	// uniform distribution for R in Metropolis criterion
	uniform_real_distribution<double> distribution(0.0, 1.0);
	// Range [1,4] for random int generator in order to choose steiner point method
	uniform_int_distribution<int> steiner_choice(1, 4);

	while(temperature >= 0 && count_obtuse_angles(cdt, polygon)) {
		for(Face_handle f : cdt.finite_face_handles()){
			// if face is not obtuse, continue
			if(!has_obtuse_angle(f)) {
				continue;
			}
			CDT triangulation = cdt;
			// For every obtuse triangle:
			// Randomly select a Steiner point insertion method
			int option = steiner_choice(generator);
			Point steiner_point = choose_steiner_point(triangulation, polygon, option);

			// If, for any reason, insertion fails, continue
			if(steiner_point == Point()){
				continue;
			}

			// Calculate delta_energy
			double new_energy = compute_energy(triangulation, polygon, steiner.size() + 1);
			double delta_energy = new_energy - energy;
			long double expon = exp(-delta_energy / temperature);
			long double r = distribution(generator);

			if(delta_energy < 0 || expon > r) {
				// delta_energy < 0, we accept the new configuration
				// else we accept with probability e^{−∆E/T} (Metropolis Criterion)
				energy = new_energy;
				cdt = triangulation;
				// Store inserted steiner in vector
				steiner.emplace_back(steiner_point);
				break;
			}

		}
		
		temperature -= (1.0 / L);
	}
}
