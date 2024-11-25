#include "local_search.h"

void local_search_optimization(CDT& cdt, Polygon_2& polygon, int max_iterations, vector<Point>& steiner) {
	int iterations = 0;
	int obtuse_count;

	while ( (obtuse_count = count_obtuse_angles(cdt)) && iterations < max_iterations) {
		bool improved = false;
		CDT cdt_circumcenter = cdt;
		CDT cdt_projection = cdt;
		CDT cdt_midpoint = cdt;
		CDT best_triangulation = cdt;
		int best_obtuse_count = obtuse_count;

		vector<Point> steiner_points = {
			insert_circumcenter(cdt_circumcenter, polygon),
			insert_projection(cdt_projection, polygon),
			insert_midpoint(cdt_midpoint, polygon),
		};

		Point final_point;
		for (const auto& steiner_point : steiner_points) {
			if (steiner_point == Point()) 
				continue; 
				
			CDT temp_triangulation = cdt;
			temp_triangulation.insert(steiner_point);

			int new_obtuse_count = count_obtuse_angles(temp_triangulation);
			if (new_obtuse_count <= best_obtuse_count) {
				best_triangulation = temp_triangulation;
				best_obtuse_count = new_obtuse_count;
				improved = true;
				final_point = steiner_point;
			}

		}
		if (improved){
			cdt = best_triangulation;
			steiner.emplace_back(final_point);
		} else {
			break; // No further improvements
		}

		iterations++;
	}
}