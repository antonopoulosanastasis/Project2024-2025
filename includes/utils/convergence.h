#ifndef CONVERGENCE_H
#define CONVERGENCE_H

#include <map>
#include <cmath> // For std::log

using namespace std;

// Function to calculate the mean convergence value
double calculate_convergence(const map<int,int>& obtuse_counts);

#endif // CONVERGENCE_H