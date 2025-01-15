#include "local_search.h"


void local_search_opt(CDT& cdt, Polygon_2& polygon, int max_iterations, vector<Point>& steiner, map<Point, int>& index, long double& convergence_value) {
	int iterations = 0;
	int obtuse_count;
	map<int, int> obtuse_counts; // Vector to hold obtuse count every time we insert a steiner point
	int random_points = count_obtuse_angles(cdt, polygon) / 4;
	while ( (obtuse_count = count_obtuse_angles(cdt, polygon)) && iterations < max_iterations) {
		int best_obtuse_count = obtuse_count;
		bool point_added_this_iteration = false;  // Track if we add a point in this iteration
		for (Face_handle face : cdt.finite_face_handles()) {
			int obtuse_index = find_obtuse_angle_index(face);
			if(obtuse_index == -1 || is_point_outside_polygon(polygon, get_centroid(face))) {
				continue;
			}
			CDT cdt_projection = cdt;
			CDT cdt_midpoint = cdt;
			CDT cdt_circumcenter = cdt;
			CDT cdt_centroid = cdt;
			CDT cdt_adjacent = cdt;
			CDT cdt_random = cdt;

			Point p_insert = steiner_projection_at_face(face, polygon);
			if( !((p_insert.x() == 0.5) && (p_insert.y() == 0.5))) {
				cdt_projection.insert(p_insert);
				int new_obtuse_count = count_obtuse_angles(cdt_projection, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_projection;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(p_insert);
					index[p_insert] = index.size();
					obtuse_counts[steiner.size()] = best_obtuse_count;
					point_added_this_iteration = true;
					// cout << "Inserted projection at iteration " << iterations << endl;
					break;
				}
			}
			Point m_insert = steiner_midpoint_at_face(face, polygon);
			if( !((m_insert.x() == 0.5) && (m_insert.y() == 0.5)) ) {
				cdt_midpoint.insert(m_insert);
				int new_obtuse_count = count_obtuse_angles(cdt_midpoint, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_midpoint;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(m_insert);
					index[m_insert] = index.size();
					obtuse_counts[steiner.size()] = best_obtuse_count;
					point_added_this_iteration = true;
					// cout << "Inserted midpoint at iteration " << iterations << endl;
					break;
				}
			}
			Point c_insert = steiner_circumcenter_at_face(cdt_circumcenter, face, polygon);
			if( !((c_insert.x() == 0.5) && (c_insert.y() == 0.5))) {
				int new_obtuse_count = count_obtuse_angles(cdt_circumcenter, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_circumcenter;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(c_insert);
					index[c_insert] = index.size();
					obtuse_counts[steiner.size()] = best_obtuse_count;
					point_added_this_iteration = true;
					// cout << "Inserted circumcenter at iteration " << iterations << endl;
					break;
				}
			}
			Point ce_insert = steiner_centroid_at_face(face, polygon);
			if( !((ce_insert.x() == 0.5) && (ce_insert.y() == 0.5))) {
				cdt_centroid.insert(ce_insert);
				int new_obtuse_count = count_obtuse_angles(cdt_centroid, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_centroid;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(ce_insert);
					index[ce_insert] = index.size();
					obtuse_counts[steiner.size()] = best_obtuse_count;
					point_added_this_iteration = true;
					// cout << "Inserted centroid at iteration " << iterations << endl;
					break;
				}
			}
			Point a_insert = steiner_adjacent_at_face(cdt_adjacent, face, polygon);
			if( !((a_insert.x() == 0.5) && (a_insert.y() == 0.5))) {
				int new_obtuse_count = count_obtuse_angles(cdt_adjacent, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_adjacent;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(a_insert);
					index[a_insert] = index.size();
					obtuse_counts[steiner.size()] = best_obtuse_count;
					point_added_this_iteration = true;
					// cout << "Inserted adjacent at iteration " << iterations << endl;
					break;
				}
			}
		}
		// If no point was added in this iteration, add random point
        if (!point_added_this_iteration) {
			if(random_points > 0) {
				for (Face_handle face : cdt.finite_face_handles()) {
					int obtuse_index = find_obtuse_angle_index(face);
					if(obtuse_index == -1 || is_point_outside_polygon(polygon, get_centroid(face))) {
						continue;
					}
					Point r_insert = steiner_random_at_face(face, polygon);
					if( !((r_insert.x() == 0.5) && (r_insert.y() == 0.5))) {
						// make sure we dont insert a point out of bounds
						// as the offset could set the centroid out of bounds
						if(is_point_outside_polygon(polygon, r_insert)) {
							r_insert = get_centroid(face);
						}
						cdt.insert(r_insert);
						best_obtuse_count = count_obtuse_angles(cdt, polygon);
						steiner.emplace_back(r_insert);
						index[r_insert] = index.size();
						obtuse_counts[steiner.size()] = best_obtuse_count;
						random_points--;
						break;
					}
				}
			}
			else{
            	break;
			}
        }
		iterations++;
	}
	convergence_value = calculate_convergence(obtuse_counts);
}