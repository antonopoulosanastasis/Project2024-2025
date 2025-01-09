#include "convergence.h"
#include <iostream>

// Function to calculate the mean convergence value
double calculate_convergence(const map<int,int>& obtuse_counts) {
	int N = obtuse_counts.size(); // Number of iterations
	if (N <= 1) {
		return 0.0;
	}
	long double sum = 0.0;
	for (int n = 1; n <= N - 1; n++) { 
		// Check for invalid values before computing the ratios
		if (obtuse_counts.at(n) <= 0 || obtuse_counts.at(n + 1) <= 0) {
			continue;
		} 
		double obtuse_ratio = log(static_cast<double>(obtuse_counts.at(n + 1)) / obtuse_counts.at(n));
		double steiner_ratio = log(static_cast<double>(n + 1) / n);

		// Update the summation
		sum += obtuse_ratio / steiner_ratio;
	}

	// Return the mean value
	return sum / (N - 1);
}