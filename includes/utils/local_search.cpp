#include "local_search.h"


void local_search_opt(CDT& cdt, Polygon_2& polygon, int max_iterations, vector<Point>& steiner, map<Point, int>& index, long double& convergence_value) {
	int iterations = 0;
	int obtuse_count;
	vector<int> obtuse_counts; // Vector to hold obtuse count every time we insert a steiner point

	while ( (obtuse_count = count_obtuse_angles(cdt, polygon)) && iterations < max_iterations) {
		int best_obtuse_count = obtuse_count;
		obtuse_counts.push_back(obtuse_count);
		vector<Face_handle> face_handles;
		for (Face_handle face : cdt.finite_face_handles()) {
			face_handles.push_back(face);
		}
		remove_faces_outside_boundary(face_handles, polygon);
		for(int i = 0; i < face_handles.size(); i++) {
			int obtuse_index = find_obtuse_angle_index(face_handles[i]);
			if(obtuse_index == -1) {
				continue;
			}
			CDT cdt_projection = cdt;
			CDT cdt_midpoint = cdt;
			CDT cdt_circumcenter = cdt;
			CDT cdt_centroid = cdt;
			CDT cdt_adjacent = cdt;
			CDT cdt_random = cdt;
			CDT best_triangulation = cdt;
			Point p_insert = steiner_projection_at_face(face_handles[i], polygon);
			if( !((p_insert.x() == 0.5) && (p_insert.y() == 0.5))) {
				cdt_projection.insert(p_insert);
				int new_obtuse_count = count_obtuse_angles(cdt_projection, polygon);
				if (new_obtuse_count <= best_obtuse_count) {
					cdt = cdt_projection;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(p_insert);
					index[p_insert] = index.size();
					obtuse_counts.push_back(best_obtuse_count);
					// cout << "Inserted projection at iteration " << iterations << endl;
					break;
				}
			}
			Point m_insert = steiner_midpoint_at_face(face_handles[i], polygon);
			if( !((m_insert.x() == 0.5) && (m_insert.y() == 0.5)) ) {
				cdt_midpoint.insert(m_insert);
				int new_obtuse_count = count_obtuse_angles(cdt_midpoint, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_midpoint;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(m_insert);
					index[m_insert] = index.size();
					obtuse_counts.push_back(best_obtuse_count);
					// cout << "Inserted midpoint at iteration " << iterations << endl;
					break;
				}
			}
			Point c_insert = steiner_circumcenter_at_face(cdt_circumcenter, face_handles[i], polygon);
			if( !((c_insert.x() == 0.5) && (c_insert.y() == 0.5))) {
				int new_obtuse_count = count_obtuse_angles(cdt_circumcenter, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_circumcenter;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(c_insert);
					index[c_insert] = index.size();
					obtuse_counts.push_back(best_obtuse_count);
					// cout << "Inserted circumcenter at iteration " << iterations << endl;
					break;
				}
			}
			Point ce_insert = steiner_centroid_at_face(face_handles[i], polygon);
			if( !((ce_insert.x() == 0.5) && (ce_insert.y() == 0.5))) {
				cdt_centroid.insert(ce_insert);
				int new_obtuse_count = count_obtuse_angles(cdt_centroid, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_centroid;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(ce_insert);
					index[ce_insert] = index.size();
					obtuse_counts.push_back(best_obtuse_count);
					// cout << "Inserted centroid at iteration " << iterations << endl;
					break;
				}
			}
			Point a_insert = steiner_adjacent_at_face(cdt_adjacent, face_handles[i], polygon);
			if( !((a_insert.x() == 0.5) && (a_insert.y() == 0.5))) {
				int new_obtuse_count = count_obtuse_angles(cdt_adjacent, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_adjacent;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(a_insert);
					index[a_insert] = index.size();
					obtuse_counts.push_back(best_obtuse_count);
					// cout << "Inserted adjacent at iteration " << iterations << endl;
					break;
				}
			}
			Point r_insert = steiner_random_at_face(face_handles[i], polygon);
			if( !((r_insert.x() == 0.5) && (r_insert.y() == 0.5))) {
				cdt_random.insert(r_insert);
				int new_obtuse_count = count_obtuse_angles(cdt_random, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_random;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(r_insert);
					index[r_insert] = index.size();
					obtuse_counts.push_back(best_obtuse_count);
					// cout << "Inserted random at iteration " << iterations << endl;
					break;
				}
			}
		}
		iterations++;
	}
	convergence_value = calculate_convergence(obtuse_counts);
}