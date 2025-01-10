#include <CGAL/draw_constrained_triangulation_2.h>
#include <fstream>
#include <cassert>
#include <iostream>
#include <gmp.h>				// For output.json fractions
#include <sstream>
#include <boost/json/src.hpp> 	// Necessary for Boost.JSON
#include <boost/json/value.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/parse.hpp>
#include <boost/filesystem.hpp>
#include <map>					// Necessary for vertex indices
#include <string>
#include <iomanip>				// for setw
#include <pthread.h>			// for threads
#include <atomic>				// flag for directory processing
#include <queue>				// queue for threads

#include "obtuse.h"
#include "definitions.h"
#include "brute_force.h"
#include "local_search.h"
#include "simulated_annealing.h"
#include "ant_colony.h"
#include "custom_cdt.h"
#include "case_identification.h"

#define THREADS 5

namespace json = boost::json;
namespace fs = boost::filesystem;
using namespace std;

// Shared data
queue<string> fileQueue;
pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queueCond = PTHREAD_COND_INITIALIZER;
atomic<bool> directoryProcessingDone(false);	// initialized with false

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

string rational_to_string(const K::FT& coord) {
	string to_return;
	auto exact_coord = CGAL::exact(coord);

	// Convert the exact coordinate to a GMP rational (mpq_t)
	const mpq_t* gmpq_ptr = reinterpret_cast<mpq_t*>(&exact_coord);

	// Declare GMP integers to hold the numerator and denominator
	mpz_t num, den;
	mpz_init(num);
	mpz_init(den);

	// Extract the numerator and denominator using GMP functions
	mpq_get_num(num, *gmpq_ptr);  // Get the numerator
	mpq_get_den(den, *gmpq_ptr);  // Get the denominator

	// check if number is integer
	if (mpz_divisible_p(num, den)) {
		mpz_t result;
		mpz_init(result);
		mpz_divexact(result, num, den);  // Exact division, guaranteed no remainder
		to_return = mpz_get_str(nullptr, 10, result);  // Convert result to string
		mpz_clear(result);
	} else {
		to_return = mpz_get_str(nullptr, 10, num) + string("/") + mpz_get_str(nullptr, 10, den);
	}

	// Clear GMP integers
	mpz_clear(num);
	mpz_clear(den);

	return to_return;
} 

// Function to create output.json
void export_to_json(const CDT& cdt, vector<Point_2>& points, const string& filename, json::string& instance_uid, map<Point, int>& vertex_indices, const Polygon_2 polygon, int obtuse_count, boost::json::string method, boost::json::object parameters) {
	json::object json_output;
	json::array steiner_points_x, steiner_points_y, edge_array;
	vector<pair<int, int>> edges;

	for (const auto& point : points) {

		// Here we use emplace_back instead of push_back so we won't have
		// to create the object before inserting it in the array
		steiner_points_x.emplace_back(rational_to_string(point.x()));
		steiner_points_y.emplace_back(rational_to_string(point.y()));
	}

	for (auto edge = cdt.edges_begin(); edge != cdt.edges_end(); ++edge) {
		Vertex_handle v1 = edge->first->vertex(cdt.cw(edge->second));
		Vertex_handle v2 = edge->first->vertex(cdt.ccw(edge->second));
		Point p1 = v1->point();
		Point p2 = v2->point();

		int index1 = vertex_indices.at(p1);
		int index2 = vertex_indices.at(p2);

		// Calculate the midpoint of the edge
		Point_2 midpoint = CGAL::midpoint(v1->point(), v2->point());
		if(!is_point_outside_polygon(polygon, midpoint)) {
			edges.emplace_back(index1, index2);
		}

		
	}

	// Convert each edge pair to a JSON array and add it to the edge_array
	for (const auto& edge : edges) {
		json::array edge_pair = {edge.first, edge.second};
		edge_array.push_back(edge_pair);
	}

	json_output["content_type"] = "CG_SHOP_2025_Solution";
	json_output["instance_uid"] = instance_uid;
	json_output["steiner_points_x"] = steiner_points_x;
	json_output["steiner_points_y"] = steiner_points_y;
	json_output["edges"] = edge_array;
	json_output["obtuse_count"] = obtuse_count;
	json_output["method"] = method;
	json_output["parameters"] = parameters;

	ofstream file(filename);
	file << json::serialize(json_output);
}

// Process a single file
void process_file(const string& filename) {
	ifstream in_file(filename);
	if (!in_file) {
		cerr << "Error: Could not open file " << filename << endl;
		return;
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
	vector<Point_2> steiner;
	vector<int> boundary_vector;

	// Construct the polygon using the region_boundary indices
	Polygon_2 polygon;
	for (const auto& idx : region_boundary) {
		polygon.push_back(points[idx.as_int64()]);
		boundary_vector.push_back(idx.as_int64());
	}

	for (size_t i = 0; i < polygon.size(); ++i) {
		cdt.insert_constraint(polygon[i], polygon[(i + 1) % polygon.size()]);
	}

	// Initialize point indexes
	map<Point, int> vertex_indices;
	// Insert points into the triangulation 
	for (const Point& p : points) {
		cdt.insert(p);
		vertex_indices[p] = vertex_indices.size();
	}

	// Insert constrained edges based on the provided indices
	for (const auto& constraint : constraints) {
		cdt.insert_constraint(points[constraint.first], points[constraint.second]);
	}

	string case_result = identify_case(cdt, polygon, json_data["num_constraints"].as_int64(), constraints, boundary_vector, points);

	vector<Point_2> steiner2;
	if(case_result == "E") {
		int L = 200;
		long double convergence_value = 0.0;
		int obtuse_before = count_obtuse_angles(cdt, polygon);
		simulated_annealing_opt(cdt, polygon, steiner2, 3, 0.5, L, vertex_indices, convergence_value);
		int obtuse_after = count_obtuse_angles(cdt, polygon);
		int width = 25;  //

		cout << left << setw(80) << filename  << setw(width) << obtuse_before << setw(width)  << obtuse_after  << setw(width) << steiner2.size()
		  << setw(width) << convergence_value << setw(width) << 3 * obtuse_after + 0.5 * steiner2.size() << endl;
		
	}
    
}

void* workerThread(void* arg) {
	while (1) {
        string file;

		// Lock the queue to retrieve a task
		pthread_mutex_lock(&queueMutex);

		// Wait until there is work or directory processing is done
		while (fileQueue.empty() && !directoryProcessingDone) {
			pthread_cond_wait(&queueCond, &queueMutex);
		}

		// Exit if processing is done and queue is empty
		if (fileQueue.empty() && directoryProcessingDone) {
			pthread_mutex_unlock(&queueMutex);
			break;
		}

		// Get the next file to process
		file = fileQueue.front();
		fileQueue.pop();

		pthread_mutex_unlock(&queueMutex);

		// Process the file
		process_file(file);
	}

	return nullptr;
}

// Process all JSON files in a directory
void process_directory(const string& directory_path) {
	fs::path dir_path(directory_path);
	// Process directory creates 10 threads through which, it scans 10 files at a time
	pthread_t th[THREADS];
	for(int i = 0; i < THREADS; i++) {
		pthread_create(th + i, nullptr, &workerThread, nullptr);
	}


	if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
		cerr << "Error: " << directory_path << " is not a valid directory." << endl;
		return;
	}

	int width = 25;
	// cout << "File " << "\t\t\t\t\t\t\t" << "\t\t\t\tCase " << endl;
	cout << left << setw(80) << "File:"  << setw(width) << "Before" << setw(width)  << "After"  << setw(width) << "Steiner"  
		<< setw(width) << "convergence_value" << setw(40) << "Energy" << endl;
	for (const auto& entry : fs::directory_iterator(dir_path)) {
		if (fs::is_regular_file(entry) && entry.path().extension() == ".json") {
			string file = entry.path().string();
			pthread_mutex_lock(&queueMutex);
			fileQueue.push(file);
			pthread_cond_signal(&queueCond); // Notify one thread that work is available
			pthread_mutex_unlock(&queueMutex);
		}
	}
	// Mark directory processing as complete
	pthread_mutex_lock(&queueMutex);
	directoryProcessingDone = true;
	pthread_cond_broadcast(&queueCond);		// Notify all threads to finish
	pthread_mutex_unlock(&queueMutex);

	// Wait for all threads to finish
    for (int i = 0; i < THREADS; ++i) {
        pthread_join(th[i], nullptr);
    }
}

int main(int argc, char* argv[])
{

	// // Check arguments
	// if (argc < 4) {
	// 	cerr << "Error \n";
	// 	return 1;
	// }

	// string filename, output;
	// if((strcmp(argv[1], "-i") == 0) && (strcmp(argv[3], "-o") == 0)) {
	// 	filename = argv[2];
	// 	output = argv[4];
	// }
	// else if ((strcmp(argv[1], "-o") == 0) && (strcmp(argv[3], "-i") == 0)) {
	// 	filename = argv[4];
	// 	output = argv[2];
	// }
	
	// // Read the file
	// ifstream in_file(filename);
	// if (!in_file) {
	// 	cerr << "Error: Could not open file " << filename << endl;
	// 	return 1;
	// }

	// stringstream buffer;
	// buffer << in_file.rdbuf();
	// json::value json_value = json::parse(buffer.str());

	// // Extract data from the JSON
	// json::object json_data = json_value.as_object();
	// json::string instance_uid = json_data["instance_uid"].as_string();
	// json::array points_x = json_data["points_x"].as_array();
	// json::array points_y = json_data["points_y"].as_array();
	// json::array region_boundary = json_data["region_boundary"].as_array();
	// json::array additional_constraints = json_data["additional_constraints"].as_array();
	// bool delaunay = json_value.at("delaunay").as_bool();
	// json::string method = json_value.at("method").as_string();
	// boost::json::object parameters = json_value.at("parameters").as_object();

	// // Deserialize points and constraints
	// vector<Point> points = deserialize_points(points_x, points_y);
	// vector<pair<int, int>> constraints = deserialize_constraints(additional_constraints);

	// // Initialize the Constrained Delaunay Triangulation (CDT)
	// CDT cdt;
	// vector<Point_2> steiner;
	// vector<int> boundary_vector;

	// // Construct the polygon using the region_boundary indices
	// Polygon_2 polygon;
	// for (const auto& idx : region_boundary) {
	// 	polygon.push_back(points[idx.as_int64()]);
	// 	boundary_vector.push_back(idx.as_int64());
	// }

	// for (size_t i = 0; i < polygon.size(); ++i) {
	// 	cdt.insert_constraint(polygon[i], polygon[(i + 1) % polygon.size()]);
	// }

	// // Initialize point indexes
	// map<Point, int> vertex_indices;
	// // Insert points into the triangulation 
	// for (const Point& p : points) {
	// 	cdt.insert(p);
	// 	vertex_indices[p] = vertex_indices.size();
	// }

	// // Insert constrained edges based on the provided indices
	// for (const auto& constraint : constraints) {
	// 	cdt.insert_constraint(points[constraint.first], points[constraint.second]);
	// }
    
	// if (is_obtuse_triangulation(cdt)) {
	// 	cout << "The triangulation contains at least one obtuse triangle.\n";
	// } else {
	// 	cout << "All triangles in the triangulation are acute or right-angled.\n";
	// }

	// cout << "Obtuse angle count: "<< count_obtuse_angles(cdt, polygon) << '\n';

	// if (!delaunay) {
	// 	cout << "Delaunay is false" << '\n';
	// 	cout << "brute force for 4 steiner" << endl;
	// 	brute_force_steiner_insertion(cdt, 4, polygon, steiner, vertex_indices);
	// }

	// string result = identify_case(cdt, polygon, json_data["num_constraints"].as_int64(), constraints, boundary_vector, points);

	// vector<Point_2> steiner2;

	// if( method == "local" ) {
	// 	cout << "Using Local Search" << '\n';
    // 	int L = parameters.at("L").as_int64();
	// 	local_search_opt(cdt, polygon, L, steiner2, vertex_indices);
	// }
	// else if ( method == "sa" ) {
	// 	cout << "Using Simulated Annealing" << '\n';
	// 	double alpha = parameters.at("alpha").as_double();
	// 	double beta = parameters.at("beta").as_double();
	// 	int L = parameters.at("L").as_int64();
	// 	simulated_annealing_opt(cdt, polygon, steiner2, alpha, beta, L, vertex_indices);
	// } else if ( method == "ant" ) {
	// 	cout << "Using Ant Colony" << endl;
	// 	double alpha = parameters.at("alpha").as_double();
	// 	double beta = parameters.at("beta").as_double();
	// 	double xi = parameters.at("xi").as_int64();
	// 	double psi = parameters.at("psi").as_int64();
	// 	double lambda = parameters.at("lambda").as_double();
	// 	int kappa = parameters.at("kappa").as_int64();
	// 	int L = parameters.at("L").as_int64();
	// 	ant_colony_optimization(cdt, polygon, steiner2, alpha, beta, xi, psi, lambda, kappa, L, vertex_indices);
	// } else {
	// 	throw invalid_argument("Invalid minimization option");
	// }

	// int obtuse_count = count_obtuse_angles(cdt, polygon);

	// cout << "Obtuse angle count: "<< obtuse_count << '\n';

	// steiner.insert(steiner.end(), steiner2.begin(), steiner2.end());

	// export_to_json(cdt, steiner, output, instance_uid, vertex_indices, polygon, obtuse_count, method, parameters);

	// cout << "steiner points added: " << steiner.size() << endl;

	// // Draw the triangulation using CGAL's draw function
	// CGAL::draw(cdt);
	process_directory("challenge_instances/");
	return 0;
}