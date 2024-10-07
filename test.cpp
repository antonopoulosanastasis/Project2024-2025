#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_constrained_triangulation_2.h>
#include <boost/json/src.hpp> // Necessary for Boost.JSON
#include <boost/json/value.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/parse.hpp>
#include <fstream>
#include <sstream>
#include <cassert>
#include <iostream>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Exact_predicates_tag                               Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, CGAL::Default, Itag> CDT;
typedef CDT::Point Point;
typedef CDT::Edge  Edge;

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

int main(int argc, char* argv[])
{

    // Check if filename is provided as an argument
    if (argc < 2) {
        std::cerr << "Error: No input filename provided.\n";
        std::cerr << "Usage: " << argv[0] << " <input_filename>\n";
        return 1;
    }

    // Get the filename from the command-line argument
    std::string filename = argv[1];

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
    // Draw the triangulation using CGAL's draw function
    CGAL::draw(cdt);
    return 0;
}