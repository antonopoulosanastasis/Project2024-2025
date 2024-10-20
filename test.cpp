#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
//#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_constrained_triangulation_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/enum.h>
#include <boost/json/src.hpp> // Necessary for Boost.JSON
#include <boost/json/value.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/parse.hpp>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iostream>
#include <vector>
#include <cstdlib>

#include "custom_cdt.h"

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Exact_predicates_tag Itag;
typedef Custom_Constrained_Delaunay_triangulation_2<K, CGAL::Default, Itag> CDT;
typedef CDT::Point Point;
typedef CDT::Edge Edge;
typedef CDT::Face_handle Face_handle;
typedef K::Point_2 Point_2;
typedef CGAL::Polygon_2<K> Polygon_2;

namespace json = boost::json;

// Function to deserialize points from JSON arrays
std::vector<Point> deserialize_points(const json::array& points_x, const json::array& points_y) {
    std::vector<Point> points;
    for (std::size_t i = 0; i < points_x.size(); ++i) {
        int x = points_x[i].as_int64();
        int y = points_y[i].as_int64();
        points.push_back(Point(x, y));
    }
    return points;
}

// Function to deserialize constraints from JSON
std::vector<std::pair<int, int>> deserialize_constraints(const json::array& json_constraints) {
    std::vector<std::pair<int, int>> constraints;
    for (const auto& json_constraint : json_constraints) {
        int first = json_constraint.at(0).as_int64();
        int second = json_constraint.at(1).as_int64();
        constraints.push_back({first, second});
    }
    return constraints;
}

// Helper function to check if the angle at p1 between p0 and p2 is obtuse
bool is_obtuse_angle(const Point& p0, const Point& p1, const Point& p2) {
    // Vector p0 -> p1 and p2 -> p1
    K::Vector_2 v1 = p0 - p1;
    K::Vector_2 v2 = p2 - p1;
    
    // Compute the dot product
    auto dot_product = v1 * v2;
    
    // The angle is obtuse if the dot product is negative
    return dot_product < 0;
}

// Function to check if a face contains any obtuse angles
bool has_obtuse_angle(Face_handle f) {
    // Get the three vertices of the face
    Point p0 = f->vertex(0)->point();
    Point p1 = f->vertex(1)->point();
    Point p2 = f->vertex(2)->point();
    
    // Check all three angles
    if (is_obtuse_angle(p1, p0, p2)) {
        return true;  // Angle at p0 is obtuse
    }
    if (is_obtuse_angle(p0, p1, p2)) {
        return true;  // Angle at p1 is obtuse
    }
    if (is_obtuse_angle(p0, p2, p1)) {
        return true;  // Angle at p2 is obtuse
    }

    return false;  // No obtuse angles found
}

// Function to check if a triangulation is obtuse
bool is_obtuse_triangulation(CDT cdt) {
    for (Face_handle f : cdt.finite_face_handles()) {
        if (has_obtuse_angle(f)) {
            return true; // Found a face with an obtuse angle, exit early
        }
    }
    return false; // No obtuse angles found in any faces
}

// Function to count obtuse angles of a triangulation 
int count_obtuse_angles(CDT cdt) {
    int count = 0;
    for (Face_handle f : cdt.finite_face_handles()) {
        if (has_obtuse_angle(f)) {
            count++; // Found a face with an obtuse angle, exit early
        }
    }
    return count; // No obtuse angles found in any faces
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

// Helper function to find the index of the obtuse angle in a face
int find_obtuse_angle_index(Face_handle f) {
    // Get the vertices of the face
    Point p0 = f->vertex(0)->point();
    Point p1 = f->vertex(1)->point();
    Point p2 = f->vertex(2)->point();
    
    // Check which angle is obtuse
    if (is_obtuse_angle(p1, p0, p2)) {
        return 0;  // Angle at vertex 0 is obtuse
    }
    if (is_obtuse_angle(p0, p1, p2)) {
        return 1;  // Angle at vertex 1 is obtuse
    }
    if (is_obtuse_angle(p0, p2, p1)) {
        return 2;  // Angle at vertex 2 is obtuse
    }
    return -1;  // No obtuse angle
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
                std::cout << "Flipped edge opposite to obtuse angle in triangle.\n";
                return;
            } else {
                std::cout << "Edge is not flippable.\n";
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

// Function to insert a point on the edge opposite to the obtuse angle
void insert_midpoint(CDT& cdt) {
    // Iterate over each face and check for obtuse angles
    for (Face_handle f : cdt.finite_face_handles()) {
        int obtuse_index = find_obtuse_angle_index(f);
        if (obtuse_index != -1) {  // If there is an obtuse angle in the face
            // Get the two vertices opposite the obtuse angle
            Point opposite_p1 = f->vertex((obtuse_index + 1) % 3)->point();
            Point opposite_p2 = f->vertex((obtuse_index + 2) % 3)->point();

            // Compute the midpoint of the edge opposite the obtuse angle
            Point midpoint = CGAL::midpoint(opposite_p1, opposite_p2);

            // Insert the midpoint into the triangulation
            cdt.insert(midpoint);
            std::cout << "Inserted point at (" << midpoint.x() << ", " << midpoint.y() << ") to break up obtuse triangle.\n";
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
void insert_foot_of_altitude(CDT& cdt) {
    // Iterate over each face and check for obtuse angles
    for (Face_handle f : cdt.finite_face_handles()) {
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
            std::cout << "Inserted foot of altitude at (" << foot.x() << ", " << foot.y() << ") to break up obtuse triangle.\n";
            return;
        }
    }
}

// Function to insert the circumcenter or centroid of the face with an obtuse angle
void insert_circumcenter(CDT& cdt, const Polygon_2& polygon) {
    // Iterate over each face and check for obtuse angles
    for (Face_handle f : cdt.finite_face_handles()) {
        int obtuse_index = find_obtuse_angle_index(f);
        if (obtuse_index != -1) {  // If there is an obtuse angle in the face
            // Compute the circumcenter of the triangle
            Point circumcenter = get_circumcenter(f);

            // Check if the circumcenter is inside or on the boundary of the polygon
            CGAL::Bounded_side circumcenter_location = CGAL::bounded_side_2(polygon.vertices_begin(), polygon.vertices_end(), circumcenter, K());

            if (circumcenter_location != CGAL::ON_UNBOUNDED_SIDE) {
                // If the circumcenter is inside or on the boundary, insert it
                cdt.insert(circumcenter);
                //std::cout << "Inserted circumcenter at (" << circumcenter.x() << ", " << circumcenter.y() << ") to break up obtuse triangle.\n";
            } else {
                // Otherwise, compute and insert the centroid
                Point centroid = get_centroid(f);
                cdt.insert(centroid);
                //std::cout << "Inserted centroid at (" << centroid.x() << ", " << centroid.y() << ") to break up obtuse triangle.\n";
            }
            return;
        }
    }
}

// Function to apply a given sequence of insertions to the triangulation
void apply_best_sequence(CDT& cdt, Polygon_2& polygon,  const std::vector<std::string>& sequence) {
    for (const std::string& step : sequence) {
        if (step == "insert_circumcenter") {
            insert_circumcenter(cdt, polygon);
        } else if (step == "insert_midpoint") {
            insert_midpoint(cdt);
        } else if (step == "insert_foot_of_altitude") {
            insert_foot_of_altitude(cdt);
        }
        std::cout << "Applied " << step << "\n";
    }
}

void try_combinations(CDT& cdt, Polygon_2& polygon, int max_depth, int current_depth, 
                      int& min_obtuse_angles, std::vector<std::string>& best_sequence, 
                      std::vector<std::string>& current_sequence, 
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
    insert_midpoint(cdt);
    current_sequence.push_back("insert_midpoint");
    try_combinations(cdt, polygon, max_depth, current_depth + 1, min_obtuse_angles, best_sequence, current_sequence, min_steiner_points);
    current_sequence.pop_back();
    cdt = backup;  // Restore triangulation

    // Try inserting the foot of altitude
    insert_foot_of_altitude(cdt);
    current_sequence.push_back("insert_foot_of_altitude");
    try_combinations(cdt, polygon, max_depth, current_depth + 1, min_obtuse_angles, best_sequence, current_sequence, min_steiner_points);
    current_sequence.pop_back();
    cdt = backup;  // Restore triangulation
}

void brute_force_steiner_insertion(CDT& cdt, int max_steiner_points, Polygon_2& polygon) {
    int min_obtuse_angles = std::numeric_limits<int>::max();
    std::vector<std::string> best_sequence;
    std::vector<std::string> current_sequence;
    int min_steiner_points = max_steiner_points; // Reset for the minimum Steiner points used

    // Start recursive backtracking
    try_combinations(cdt, polygon, max_steiner_points, 0, min_obtuse_angles, best_sequence, current_sequence, min_steiner_points);

    if (!best_sequence.empty()) {
        // If a triangulation was found
        std::cout << "Minimum obtuse angles: " << min_obtuse_angles << "\n";
        std::cout << "Steiner points used: " << min_steiner_points << "\n";
        std::cout << "Best sequence of insertions for minimum obtuse triangulation: ";
        for (const std::string& step : best_sequence) {
            std::cout << step << " ";
        }
        apply_best_sequence(cdt, polygon, best_sequence);
        std::cout << "\n";
    } else {
        std::cout << "Could not reduce obtuse angles with given Steiner points.\n";
    }
}

int main(int argc, char* argv[])
{

    // Check if filename is provided as an argument
    if (argc < 3) {
        std::cerr << "Error: No input filename provided.\n";
        std::cerr << "Usage: " << argv[0] << " <input_filename>\n";
        return 1;
    }

    // Get the filename from the command-line argument
    std::string filename = argv[1];
    int steiner_points = std::stoi(argv[2]);
    // Read the file
    std::ifstream in_file(filename);
    if (!in_file) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << in_file.rdbuf();
    json::value json_value = json::parse(buffer.str());

    // Extract data from the JSON
    json::object json_data = json_value.as_object();
    json::array points_x = json_data["points_x"].as_array();
    json::array points_y = json_data["points_y"].as_array();
    json::array region_boundary = json_data["region_boundary"].as_array();
    json::array additional_constraints = json_data["additional_constraints"].as_array(); 

    // Deserialize points and constraints
    std::vector<Point> points = deserialize_points(points_x, points_y);
    std::vector<std::pair<int, int>> constraints = deserialize_constraints(additional_constraints);

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
    std::cout << "Polygon vertices: " << std::endl;
    for (const auto& vertex : polygon.vertices()) {
        std::cout << vertex << std::endl;
    }

    if (is_obtuse_triangulation(cdt)) {
        std::cout << "The triangulation contains at least one obtuse triangle.\n";
    } else {
        std::cout << "All triangles in the triangulation are acute or right-angled.\n";
    }

    int count = 0;
    std::cout << "Obtuse angle count: "<< count_obtuse_angles(cdt) << '\n';
    brute_force_steiner_insertion(cdt, steiner_points, polygon);

    if (is_obtuse_triangulation(cdt)) {
        std::cout << "The triangulation contains at least one obtuse triangle.\n";
    } else {
        std::cout << "All triangles in the triangulation are acute or right-angled.\n";
    }

    std::cout << "Steiner count: "<< count << '\n';
    std::cout << "Obtuse angle count: "<< count_obtuse_angles(cdt) << '\n';

    // Draw the triangulation using CGAL's draw function
    CGAL::draw(cdt);
    return 0;
}