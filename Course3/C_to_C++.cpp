// ============================================================================
// Program: Vector Summation
// Author:  Troy Celd
// Purpose: Demonstrates the transition from C to modern C++ by calculating 
//          the sum of a sequence of integers. Showcases C++ concepts like 
//          std::vector, pass-by-reference, inline functions, and stream I/O.
// ============================================================================

#include <iostream> // Replaces <stdio.h> for C++ stream input/output
#include <vector>   // Includes the dynamic array container from the standard library

using namespace std;

// Type-safe constant replacing the old C-style #define macro
const int N = 40; 

// The C++ version of the reference C code sum function.
// 'inline' requests the compiler to expand the function body at the call site for speed.
// We use 'int& p' (a reference) instead of a pointer to modify the original variable safely.
inline void sum(int& p, int n, std::vector<int>& d){
    int i;
    p = 0; // Reset the accumulator (no dereferencing asterisk needed!)

    for(i = 0; i < n; ++i){
        p = p + d[i];
    }
}

// Main Function
int main(void) {
    // Initialize a vector of size N to hold our integers safely
    std::vector<int> data(N);
    
    int accum = 0; 

    for(int i = 0; i < N; ++i){
        data[i] = i;
    }

    sum(accum, N, data);

    // Output the final result using C++ streams, complete with proper spacing
    std::cout << "sum is " << accum << std::endl; 
    
    return 0;
}