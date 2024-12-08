#include "brute_force.h"

// Function to apply a given sequence of insertions to the triangulation
void apply_best_sequence(CDT& cdt, Polygon_2& polygon,  const vector<string>& sequence, vector<Point_2>& steiner) {
	for (const string& step : sequence) {
		if (step == "insert_circumcenter") {
			steiner.emplace_back(insert_circumcenter(cdt, polygon));
		} else if (step == "insert_midpoint") {
			steiner.emplace_back(insert_midpoint(cdt, polygon));
		} else if (step == "insert_projection") {
			steiner.emplace_back(insert_projection(cdt, polygon));
		} else if(step == "insert_centroid") {
			steiner.emplace_back(insert_centroid(cdt, polygon));
		} else if(step == "insert_adjacent") {
			steiner.emplace_back(insert_adjacent(cdt, polygon));
		}
		cout << "Applied " << step << "\n";
	}
}

void try_combinations(CDT& cdt, Polygon_2& polygon, int max_depth, int current_depth, int& min_obtuse_angles, vector<string>& best_sequence, vector<string>& current_sequence, int& min_steiner_points) {
    
	int current_obtuse_angles = count_obtuse_angles(cdt, polygon);  // Count obtuse angles in the current triangulation
    
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

	// Try inserting projection
	insert_projection(cdt, polygon);
	current_sequence.push_back("insert_projection");
	try_combinations(cdt, polygon, max_depth, current_depth + 1, min_obtuse_angles, best_sequence, current_sequence, min_steiner_points);
	current_sequence.pop_back();
	cdt = backup;  // Restore triangulation

	// Try inserting centroid
	insert_centroid(cdt, polygon);
	current_sequence.push_back("insert_centroid");
	try_combinations(cdt, polygon, max_depth, current_depth + 1, min_obtuse_angles, best_sequence, current_sequence, min_steiner_points);
	current_sequence.pop_back();
	cdt = backup;  // Restore triangulation

	// Try inserting adjacent
	insert_adjacent(cdt, polygon);
	current_sequence.push_back("insert_adjacent");
	try_combinations(cdt, polygon, max_depth, current_depth + 1, min_obtuse_angles, best_sequence, current_sequence, min_steiner_points);
	current_sequence.pop_back();
	cdt = backup;  // Restore triangulation
}

void brute_force_steiner_insertion(CDT& cdt, int max_steiner_points, Polygon_2& polygon, vector<Point_2>& steiner) {
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
		cout << endl;
		apply_best_sequence(cdt, polygon, best_sequence, steiner);
		cout << "\n";
	} else {
		cout << "Could not reduce obtuse angles with given Steiner points.\n";
	}

}