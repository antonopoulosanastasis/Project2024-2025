#include <CGAL/draw_constrained_triangulation_2.h>
#include <fstream>
#include <cassert>
#include <iostream>
#include <CGAL/Gmpq.h>			// For output.json fractions
#include <sstream>
#include <boost/json/src.hpp> 	// Necessary for Boost.JSON
#include <boost/json/value.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/parse.hpp>

#include "obtuse.h"
#include "definitions.h"


namespace json = boost::json;
using namespace std;

// Global vector to store steiner points for output.json
vector<Point_2> steiner;

// Function to deserialize points from JSON arrays
vector<Point> deserialize_points(const json::array& points_x, const json::array& points_y) {
    vector<Point> points;
    for (size_t i = 0; i < points_x.size(); ++i) {
        int x = points_x[i].as_int64();
        int y = points_y[i].as_int64();
        points.push_back(Point(x, y));
    }
    return points;
}

// Function to deserialize constraints from JSON
vector<pair<int, int>> deserialize_constraints(const json::array& json_constraints) {
    vector<pair<int, int>> constraints;
    for (const auto& json_constraint : json_constraints) {
        int first = json_constraint.at(0).as_int64();
        int second = json_constraint.at(1).as_int64();
        constraints.push_back({first, second});
    }
    return constraints;
}

// Converts CGALL::Gmpq to int
int gmpq_to_int(const CGAL::Gmpq& value) {
	if (value.denominator() == 1) {
		return value.numerator().to_double();
	} else {
		return value.numerator().to_double() / value.denominator().to_double();
	}
}

// Function to turn a rational to fraction as a string
string to_fraction_string(const CGAL::Gmpq& rational) {
	ostringstream oss;
	oss << rational.numerator() << "/" << rational.denominator();
	return oss.str();
}

// Function to check if given value is integer
bool is_integer(const CGAL::Gmpq& value) {
	return value.numerator() % value.denominator() == 0;
}

// Function to determine the format of steiner point(fraction/integer)
string format_value(const CGAL::Gmpq& value) {
	if (is_integer(value)) {
		return to_string(gmpq_to_int(value));
	} else {
		return to_fraction_string(value); // fraction
	}
}

// Function to create output.json
void export_to_json(vector<Point_2>& points, const string& filename, json::string& instance_uid) {
	json::object json_output;
	json::array steiner_points_x, steiner_points_y;

	for (const auto& point : points) {
		// Convert coordinates to rational numbers
		CGAL::Gmpq x_rational(CGAL::to_double(point.x()));
		CGAL::Gmpq y_rational(CGAL::to_double(point.y()));

		// Here we use emplace_back instead of push_back so we won't have
		// to create the object before inserting it in the array
		steiner_points_x.emplace_back(format_value(x_rational));
		steiner_points_y.emplace_back(format_value(y_rational));
	}

	json_output["content_type"] = "CG_SHOP_2025_Solution";
	json_output["instance_uid"] = instance_uid;
	json_output["steiner_points_x"] = steiner_points_x;
	json_output["steiner_points_y"] = steiner_points_y;
	// json_output["edges"] = edges;

	ofstream file(filename);
	file << json::serialize(json_output);
}

// Function to get the circumcenter of a face (triangle)
Point get_circumcenter(Face_handle face) {
	// Get the vertices of the triangle
	Point p1 = face->vertex(0)->point();
	Point p2 = face->vertex(1)->point();
	Point p3 = face->vertex(2)->point();

	// CGAL provides a built-in function to compute the circumcenter
	return CGAL::circumcenter(p1, p2, p3);
}

// Function to get the centroid of a face (triangle)
Point get_centroid(Face_handle face) {
	// Get the vertices of the triangle
	Point p1 = face->vertex(0)->point();
	Point p2 = face->vertex(1)->point();
	Point p3 = face->vertex(2)->point();

	// CGAL provides a built-in function to compute the centroid
	return CGAL::centroid(p1, p2, p3);
}

// Function to flip the edge opposite to the obtuse angle in a face
void flip_obtuse_edges(CDT& cdt) {
    for (Face_handle f : cdt.finite_face_handles()) {
        int obtuse_index = find_obtuse_angle_index(f);
        if (obtuse_index != -1) {  // If there is an obtuse angle in the face
            // Find the index of the edge opposite to the obtuse angle
            int opposite_edge_index = (obtuse_index + 1) % 3;

            // Check if the edge is flippable
            if (1) {
                // Perform the edge flip
                cdt.flip(f, opposite_edge_index);
                cout << "Flipped edge opposite to obtuse angle in triangle.\n";
                return;
            } else {
                cout << "Edge is not flippable.\n";
            }
        }
    }
}

// Function to check if the point is outside the polygon
bool is_point_outside_polygon(const Polygon_2& polygon, const Point_2& point) {
    CGAL::Bounded_side result = CGAL::bounded_side_2(polygon.vertices_begin(), polygon.vertices_end(), point, K());

    // Return true if the point is outside the polygon
    return (result == CGAL::ON_UNBOUNDED_SIDE);
}

// Function to remove faces outside boundary from a face vector
void remove_faces_outside_boundary(vector<CDT::Face_handle>& face_vector, const Polygon_2& boundary) {
	vector<CDT::Face_handle>::iterator it = face_vector.begin();

	// Iterate through the vector and remove faces based on the condition
	while (it != face_vector.end()) {
		if (is_point_outside_polygon(boundary, get_centroid(*it)) ) {
			it = face_vector.erase(it); // Remove face and get new iterator
		} else {
			++it; // Move to the next face
		}
	}
}

// Function to insert a point on the edge opposite to the obtuse angle
void insert_midpoint(CDT& cdt, const Polygon_2& boundary) {
	// face_handles vector will store faces inside given boundary
	vector<Face_handle> face_handles;
	for (Face_handle face : cdt.finite_face_handles()) {
		face_handles.push_back(face);
	}
	remove_faces_outside_boundary(face_handles, boundary);

    // Iterate over each face and check for obtuse angles
    for (Face_handle f : face_handles) {
        int obtuse_index = find_obtuse_angle_index(f);
        if (obtuse_index != -1) {  // If there is an obtuse angle in the face
            // Get the two vertices opposite the obtuse angle
            Point opposite_p1 = f->vertex((obtuse_index + 1) % 3)->point();
            Point opposite_p2 = f->vertex((obtuse_index + 2) % 3)->point();

            // Compute the midpoint of the edge opposite the obtuse angle
            Point midpoint = CGAL::midpoint(opposite_p1, opposite_p2);

            // Insert the midpoint into the triangulation
            cdt.insert(midpoint);
			steiner.push_back(midpoint);
            cout << "Inserted point at (" << midpoint.x() << ", " << midpoint.y() << ") to break up obtuse triangle.\n";
            return;
        }
    }
}

// Function to compute the foot of the altitude from point A onto the line BC
Point foot_of_altitude(const Point& A, const Point& B, const Point& C) {
    // Vector from B to C
    K::Vector_2 BC = C - B;
    
    // Vector from B to A
    K::Vector_2 BA = A - B;
    
    // Projection of BA onto BC
    K::FT scalar_proj = (BA * BC) / (BC * BC);
    
    // Compute the foot of the altitude
    Point P = B + scalar_proj * BC;
    
    return P;
}

// Function to insert the foot of the altitude from the obtuse angle to the opposite side
void insert_foot_of_altitude(CDT& cdt, const Polygon_2& polygon) {

	// face_handles vector will store faces inside given boundary
	vector<Face_handle> face_handles;
	for (Face_handle face : cdt.finite_face_handles()) {
		face_handles.push_back(face);
	}
	remove_faces_outside_boundary(face_handles, polygon);
    // Iterate over each face and check for obtuse angles
    for (Face_handle f : face_handles) {
        int obtuse_index = find_obtuse_angle_index(f); 
        if (obtuse_index != -1) {  // If there is an obtuse angle in the face
            // Get the vertex at the obtuse angle
            Point obtuse_vertex = f->vertex(obtuse_index)->point();

            // Get the two vertices opposite the obtuse angle
            Point opposite_p1 = f->vertex((obtuse_index + 1) % 3)->point();
            Point opposite_p2 = f->vertex((obtuse_index + 2) % 3)->point();

            // Compute the foot of the altitude from the obtuse vertex to the opposite edge
            Point foot = foot_of_altitude(obtuse_vertex, opposite_p1, opposite_p2);

            // Insert the foot of the altitude into the triangulation
            cdt.insert(foot);
			steiner.push_back(foot);
            cout << "Inserted foot of altitude at (" << foot.x() << ", " << foot.y() << ") to break up obtuse triangle.\n";
            return;
        }
    }
}

// Function to insert the circumcenter or centroid of the face with an obtuse angle
void insert_circumcenter(CDT& cdt, const Polygon_2& polygon) {
	// face_handles vector will store faces inside given boundary
	vector<Face_handle> face_handles;
	for (Face_handle face : cdt.finite_face_handles()) {
		face_handles.push_back(face);
	}
	remove_faces_outside_boundary(face_handles, polygon);
    // Iterate over each face and check for obtuse angles
    for (Face_handle f : face_handles) {
        int obtuse_index = find_obtuse_angle_index(f);
        if (obtuse_index != -1) {  // If there is an obtuse angle in the face
            // Compute the circumcenter of the triangle
            Point circumcenter = get_circumcenter(f);

            // Check if the circumcenter is inside or on the boundary of the polygon
            CGAL::Bounded_side circumcenter_location = CGAL::bounded_side_2(polygon.vertices_begin(), polygon.vertices_end(), circumcenter, K());

            if (circumcenter_location != CGAL::ON_UNBOUNDED_SIDE) {
                // If the circumcenter is inside or on the boundary, insert it
                cdt.insert(circumcenter);
				steiner.push_back(circumcenter);
                //cout << "Inserted circumcenter at (" << circumcenter.x() << ", " << circumcenter.y() << ") to break up obtuse triangle.\n";
            } else {
                // Otherwise, compute and insert the centroid
                Point centroid = get_centroid(f);
                cdt.insert(centroid);
				steiner.push_back(centroid);
                //cout << "Inserted centroid at (" << centroid.x() << ", " << centroid.y() << ") to break up obtuse triangle.\n";
            }
            return;
        }
    }
}

// Function to apply a given sequence of insertions to the triangulation
void apply_best_sequence(CDT& cdt, Polygon_2& polygon,  const vector<string>& sequence) {
    for (const string& step : sequence) {
        if (step == "insert_circumcenter") {
            insert_circumcenter(cdt, polygon);
        } else if (step == "insert_midpoint") {
            insert_midpoint(cdt, polygon);
        } else if (step == "insert_foot_of_altitude") {
            insert_foot_of_altitude(cdt, polygon);
        }
        cout << "Applied " << step << "\n";
    }
}

void try_combinations(CDT& cdt, Polygon_2& polygon, int max_depth, int current_depth, 
                      int& min_obtuse_angles, vector<string>& best_sequence, 
                      vector<string>& current_sequence, 
                      int& min_steiner_points) {
    
    int current_obtuse_angles = count_obtuse_angles(cdt);  // Count obtuse angles in the current triangulation
    
    // Check if the current triangulation is better
    if (current_obtuse_angles < min_obtuse_angles || 
        (current_obtuse_angles == min_obtuse_angles && current_depth < min_steiner_points)) {
        
        min_obtuse_angles = current_obtuse_angles;
        min_steiner_points = current_depth;
        best_sequence = current_sequence; // Update the best sequence
    }

    if (current_depth >= max_depth) {
        return; // Stop recursion if max depth is reached
    }

    // Backup the current triangulation
    CDT backup = cdt;

    // Try inserting the circumcenter
    insert_circumcenter(cdt, polygon);
    current_sequence.push_back("insert_circumcenter");
    try_combinations(cdt, polygon, max_depth, current_depth + 1, min_obtuse_angles, best_sequence, current_sequence, min_steiner_points);
    current_sequence.pop_back();
    cdt = backup;  // Restore triangulation

    // Try inserting the midpoint
    insert_midpoint(cdt, polygon);
    current_sequence.push_back("insert_midpoint");
    try_combinations(cdt, polygon, max_depth, current_depth + 1, min_obtuse_angles, best_sequence, current_sequence, min_steiner_points);
    current_sequence.pop_back();
    cdt = backup;  // Restore triangulation

    // Try inserting the foot of altitude
    insert_foot_of_altitude(cdt, polygon);
    current_sequence.push_back("insert_foot_of_altitude");
    try_combinations(cdt, polygon, max_depth, current_depth + 1, min_obtuse_angles, best_sequence, current_sequence, min_steiner_points);
    current_sequence.pop_back();
    cdt = backup;  // Restore triangulation
}

void brute_force_steiner_insertion(CDT& cdt, int max_steiner_points, Polygon_2& polygon) {
    int min_obtuse_angles = numeric_limits<int>::max();
    vector<string> best_sequence;
    vector<string> current_sequence;
    int min_steiner_points = max_steiner_points; // Reset for the minimum Steiner points used

    // Start recursive backtracking
    try_combinations(cdt, polygon, max_steiner_points, 0, min_obtuse_angles, best_sequence, current_sequence, min_steiner_points);

    if (!best_sequence.empty()) {
        // If a triangulation was found
        cout << "Minimum obtuse angles: " << min_obtuse_angles << "\n";
        cout << "Steiner points used: " << min_steiner_points << "\n";
        cout << "Best sequence of insertions for minimum obtuse triangulation: ";
        for (const string& step : best_sequence) {
            cout << step << " ";
        }
        apply_best_sequence(cdt, polygon, best_sequence);
        cout << "\n";
    } else {
        cout << "Could not reduce obtuse angles with given Steiner points.\n";
    }
}

int main(int argc, char* argv[])
{

    // Check if filename is provided as an argument
    if (argc < 3) {
        cerr << "Error: No input filename provided.\n";
        cerr << "Usage: " << argv[0] << " <input_filename>\n";
        return 1;
    }

    // Get the filename from the command-line argument
    string filename = argv[1];
    int steiner_points = stoi(argv[2]);
    // Read the file
    ifstream in_file(filename);
    if (!in_file) {
        cerr << "Error: Could not open file " << filename << endl;
        return 1;
    }

    stringstream buffer;
    buffer << in_file.rdbuf();
    json::value json_value = json::parse(buffer.str());

    // Extract data from the JSON
    json::object json_data = json_value.as_object();
	json::string instance_uid = json_data["instance_uid"].as_string();
    json::array points_x = json_data["points_x"].as_array();
    json::array points_y = json_data["points_y"].as_array();
    json::array region_boundary = json_data["region_boundary"].as_array();
    json::array additional_constraints = json_data["additional_constraints"].as_array(); 

    // Deserialize points and constraints
    vector<Point> points = deserialize_points(points_x, points_y);
    vector<pair<int, int>> constraints = deserialize_constraints(additional_constraints);

    // Initialize the Constrained Delaunay Triangulation (CDT)
    CDT cdt;

    // Insert points into the triangulation 
    for (const Point& p : points) {
        cdt.insert(p);
    }

    // Insert constrained edges based on the provided indices
    for (const auto& constraint : constraints) {
        cdt.insert_constraint(points[constraint.first], points[constraint.second]);
    }
    
    // Construct the polygon using the region_boundary indices
    Polygon_2 polygon;
    for (const auto& idx : region_boundary) {
        polygon.push_back(points[idx.as_int64()]);
    }

    // You now have a polygon built from the region_boundary!
    // For example, print the vertices of the polygon:
    cout << "Polygon vertices: " << endl;
    for (const auto& vertex : polygon.vertices()) {
        cout << vertex << endl;
    }

    if (is_obtuse_triangulation(cdt)) {
        cout << "The triangulation contains at least one obtuse triangle.\n";
    } else {
        cout << "All triangles in the triangulation are acute or right-angled.\n";
    }

    // int count = 0;
    cout << "Obtuse angle count: "<< count_obtuse_angles(cdt) << '\n';
    brute_force_steiner_insertion(cdt, steiner_points, polygon);

    if (is_obtuse_triangulation(cdt)) {
        cout << "The triangulation contains at least one obtuse triangle.\n";
    } else {
        cout << "All triangles in the triangulation are acute or right-angled.\n";
    }

    // cout << "Steiner count: " << count << '\n';
    cout << "Obtuse angle count: "<< count_obtuse_angles(cdt) << '\n';

	export_to_json(steiner, "output.json", instance_uid);

    // Draw the triangulation using CGAL's draw function
    CGAL::draw(cdt);
    return 0;
}