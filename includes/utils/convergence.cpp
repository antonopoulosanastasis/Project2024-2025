#include "convergence.h"
#include <iostream>

// Function to calculate the mean convergence value
double calculate_convergence(const vector<int>& obtuse_counts) {
	int N = obtuse_counts.size(); // Number of iterations
	if(N < 2) {
		return 0.0;
	}
	long double sum = 0.0;
	for (int n = 1; n <= N - 1; n++) { 
		// Calculate the ratios for the formula
		double obtuse_ratio = static_cast<double>(obtuse_counts[n + 1]) / obtuse_counts[n];
		double steiner_ratio = static_cast<double>(n + 1) / n;

		std::cout << log(obtuse_ratio) << "   " <<  log(steiner_ratio) << std::endl;

		// Update the summation
		sum += log(obtuse_ratio) / log(steiner_ratio);
	}

	// Return the mean value
	return sum / (N - 1);
}