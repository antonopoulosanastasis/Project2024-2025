#include "simulated_annealing.h"

double compute_energy(CDT& cdt, Polygon_2& polygon, int steiner_count, double alpha, double beta) {
	return alpha * count_obtuse_angles(cdt, polygon) + beta * steiner_count;
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
		case 5:
			return insert_adjacent(cdt, polygon);
		default:
			throw invalid_argument("Invalid Steiner point option");
	}
}

Point choose_steiner(CDT& cdt, const Polygon_2& polygon, int option, Face_handle& face) {
	switch (option) {
		case 1:
			return steiner_projection_at_face(face, polygon);
		case 2:
			return steiner_midpoint_at_face(face, polygon);
		case 3:
			return steiner_circumcenter_at_face(face, polygon);
		case 4:
			return steiner_centroid_at_face(face, polygon);
		case 5:
			return steiner_adjacent_at_face(cdt, face, polygon);
		default:
			throw invalid_argument("Invalid Steiner point option");
	}
}

void simulated_annealing_optimization(CDT& cdt, Polygon_2& polygon, vector<Point>& steiner, double alpha, double beta, int L) {
	double energy = compute_energy(cdt, polygon, 0, alpha, beta);
	double temperature = 1.0;

	// Create a random device to seed the random number generator
	random_device rd;
	// Create a Mersenne Twister pseudo-random generator initialized with rd
	mt19937 gen(rd());
	// Create a uniform distribution for generating doubles between 0 and 1
	uniform_real_distribution<> distribution(0.0, 1.0);
	// Range [1,4] for random int generator in order to choose steiner point method
	uniform_int_distribution<int> steiner_choice(1, 5);

	while(temperature >= 0 && count_obtuse_angles(cdt, polygon)) {
		for(Face_handle f : cdt.finite_face_handles()){
			// if face is not obtuse, continue
			if(!has_obtuse_angle(f)) {
				continue;
			}
			CDT triangulation = cdt;
			// For every obtuse triangle:
			// Randomly select a Steiner point insertion method
			int option = steiner_choice(gen);
			Point steiner_point = choose_steiner_point(triangulation, polygon, option);

			// If, for any reason, insertion fails, continue
			if(steiner_point == Point()){
				continue;
			}

			// Calculate delta_energy
			double new_energy = compute_energy(triangulation, polygon, steiner.size() + 1, alpha, beta);
			double delta_energy = new_energy - energy;
			long double expon = exp(-delta_energy / temperature);
			long double r = distribution(gen);

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
void simulated_annealing_opt(CDT& cdt, Polygon_2& polygon, vector<Point>& steiner, double alpha, double beta, int L) {
	double energy = compute_energy(cdt, polygon, 0, alpha, beta);
	double temperature = 1.0;

	// Create a random device to seed the random number generator
	random_device rd;
	// Create a Mersenne Twister pseudo-random generator initialized with rd
	mt19937 gen(rd());
	// Create a uniform distribution for generating doubles between 0 and 1
	uniform_real_distribution<> distribution(0.0, 1.0);
	// Range [1,5] for random int generator in order to choose steiner point method
	uniform_int_distribution<int> steiner_choice(1, 5);

	while(temperature >= 0 && count_obtuse_angles(cdt, polygon)) {

		vector<Face_handle> face_handles;
		for (Face_handle face : cdt.finite_face_handles()) {
			face_handles.push_back(face);
		}
		remove_faces_outside_boundary(face_handles, polygon);
		remove_non_obtuse_faces(face_handles);

		for(Face_handle face : face_handles) {

			CDT triangulation = cdt;
			int option = steiner_choice(gen);
			Point steiner_point = choose_steiner(triangulation, polygon, option, face);
			// If, for any reason, insertion fails, continue
			if(steiner_point == Point(0.5, 0.5)){
				continue;
			}
			if(option != 5) { // if option is not adjacent
				triangulation.insert(steiner_point);
			}

			// Calculate delta_energy
			double new_energy = compute_energy(triangulation, polygon, steiner.size() + 1, alpha, beta);
			double delta_energy = new_energy - energy;
			long double expon = exp(-delta_energy / temperature);
			long double r = distribution(gen);

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