#include "local_search.h"

void local_search_opt(CDT& cdt, Polygon_2& polygon, int max_iterations, vector<Point>& steiner) {
	int iterations = 0;
	int obtuse_count;
	while ( (obtuse_count = count_obtuse_angles(cdt, polygon)) && iterations < max_iterations) {
		int best_obtuse_count = obtuse_count;
		vector<Face_handle> face_handles;
		for (Face_handle face : cdt.finite_face_handles()) {
			face_handles.push_back(face);
		}
		remove_faces_outside_boundary(face_handles, polygon);
		remove_non_obtuse_faces(face_handles);
		for(int i = 0; i < face_handles.size(); i++) {
			CDT cdt_projection = cdt;
			CDT cdt_midpoint = cdt;
			CDT cdt_circumcenter = cdt;
			CDT cdt_centroid = cdt;
			CDT cdt_adjacent = cdt;
			CDT best_triangulation = cdt;
			Point m_insert = steiner_midpoint_at_face(face_handles[i], polygon);
			if( !((m_insert.x() == 0.5) && (m_insert.y() == 0.5)) ) {
				cdt_midpoint.insert(m_insert);
				int new_obtuse_count = count_obtuse_angles(cdt_midpoint, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_midpoint;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(m_insert);
					cout << "Inserted midpoint at iteration " << iterations << endl;
					break;
				}
			}
			Point c_insert = steiner_circumcenter_at_face(face_handles[i], polygon);
			if( !((c_insert.x() == 0.5) && (c_insert.y() == 0.5))) {
				cdt_circumcenter.insert(c_insert);
				int new_obtuse_count = count_obtuse_angles(cdt_circumcenter, polygon);
				if (new_obtuse_count < best_obtuse_count) {
					cdt = cdt_circumcenter;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(c_insert);
					cout << "Inserted circumcenter at iteration " << iterations << endl;
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
					cout << "Inserted centroid at iteration " << iterations << endl;
					break;
				}
			}
			Point p_insert = steiner_projection_at_face(face_handles[i], polygon);
			if( !((p_insert.x() == 0.5) && (p_insert.y() == 0.5))) {
				cdt_projection.insert(p_insert);
				int new_obtuse_count = count_obtuse_angles(cdt_projection, polygon);
				if (new_obtuse_count <= best_obtuse_count) {
					cdt = cdt_projection;
					best_obtuse_count = new_obtuse_count;
					steiner.emplace_back(p_insert);
					cout << "Inserted projection at iteration " << iterations << endl;
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
					cout << "Inserted adjacent at iteration " << iterations << endl;
					break;
				}
			}
		}
		iterations++;
	}
}