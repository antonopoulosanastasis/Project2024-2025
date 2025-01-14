#include "simulated_annealing.h"

double compute_energy(CDT& cdt, Polygon_2& polygon, int steiner_count, double alpha, double beta) {
	return alpha * count_obtuse_angles(cdt, polygon) + beta * steiner_count;
}

Point choose_steiner(CDT& cdt, const Polygon_2& polygon, int option, Face_handle& face) {
	switch (option) {
		case 1:
			return steiner_projection_at_face(face, polygon);
		case 2:
			return steiner_midpoint_at_face(face, polygon);
		case 3:
			return steiner_circumcenter_at_face(cdt, face, polygon);
		case 4:
			return steiner_centroid_at_face(face, polygon);
		case 5:
			return steiner_adjacent_at_face(cdt, face, polygon);
		default:
			throw invalid_argument("Invalid Steiner point option");
	}
}

void simulated_annealing_opt(CDT& cdt, Polygon_2& polygon, vector<Point>& steiner, double alpha, double beta, int L, map<Point, int>& index, long double& convergence) {
	double energy = compute_energy(cdt, polygon, 0, alpha, beta);
	double temperature = 1.0;
	map<int,int> obtuse_counts; // Vector to hold obtuse count every time we insert a steiner point

	// Create a random device to seed the random number generator
	random_device rd;
	// Create a Mersenne Twister pseudo-random generator initialized with rd
	mt19937 gen(rd());
	// Create a uniform distribution for generating doubles between 0 and 1
	uniform_real_distribution<> distribution(0.0, 1.0);
	// Range [1,5] for random int generator in order to choose steiner point method
	uniform_int_distribution<int> steiner_choice(1, 5);

	int iteration_without_insertion = 0; // Counter for iterations without insertion
    const int max_iterations_without_insertion = 10; // Threshold for choosing a random face
	int random_points = count_obtuse_angles(cdt, polygon) / 4; // Number of random points to insert

	while(temperature > 0 && count_obtuse_angles(cdt, polygon)) {

		bool steiner_inserted = false; // Track if a Steiner point was inserted this iteration

		for (Face_handle face : cdt.finite_face_handles()) {
			int obtuse_index = find_obtuse_angle_index(face);
			if(obtuse_index == -1 || is_point_outside_polygon(polygon, get_centroid(face))) {
				continue;
			}
			CDT triangulation = cdt;
			int option = steiner_choice(gen);
			Point steiner_point = choose_steiner(triangulation, polygon, option, face);
			// If, for any reason, insertion fails, continue
			if(steiner_point == Point(0.5, 0.5)){
				continue;
			}
			if(option != 5 && option != 3) { // if option is not adjacent or circumcenter
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
				index[steiner_point] = index.size();
				int best_obtuse_count = count_obtuse_angles(cdt, polygon);
				obtuse_counts[steiner.size()] = best_obtuse_count;

				steiner_inserted = true; // Steiner point was inserted
                iteration_without_insertion = 0; // Reset the counter
				break;
			}
		}

		if (!steiner_inserted) {
            iteration_without_insertion++;
        }

		// If no Steiner point has been inserted for max_iterations_without_insertion, insert one randomly
        if (iteration_without_insertion >= max_iterations_without_insertion) {
			if(random_points > 0) {
				for (Face_handle face : cdt.finite_face_handles()) {
					int obtuse_index = find_obtuse_angle_index(face);
					if(obtuse_index == -1 || is_point_outside_polygon(polygon, get_centroid(face))) {
						continue;
					}
					Point random_steiner_point = steiner_random_at_face(face, polygon);
					if (random_steiner_point != Point(0.5, 0.5)) {
						cdt.insert(random_steiner_point);
						steiner.emplace_back(random_steiner_point);
						index[random_steiner_point] = index.size();
						int best_obtuse_count = count_obtuse_angles(cdt, polygon);
						obtuse_counts[steiner.size()] = best_obtuse_count;
						random_points--;
						iteration_without_insertion = 0; // Reset the counter
						break;
					}
				}
			}
        }
		temperature -= (1.0 / L);
	}
	convergence =  calculate_convergence(obtuse_counts);
}