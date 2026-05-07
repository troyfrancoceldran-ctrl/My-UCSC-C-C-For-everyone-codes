/**
 * This program prints out the sine and cosine values of an input continuous value.
 * The input-value is default in radians between 0 and 1 (exclusive).
 */

#include <stdio.h>
#include <math.h>

// PART I. INSTRUCTION FUNCTIONS --- //

// 1. COMPUTATION FUNCTION FOR SINE, computes sin(x) input
double calculate_sine(double x) {
    return sin(x);
} 

// 2. COMPUTATION FUNCTION FOR COSINE, computes cos(x) output
double calculate_cos(double x) {
    return cos(x);
}

// 3. OUTPUT DISPLAY FUNCTION, responsible for printing out the values of all inputs and outputs
void result(double x, double sin_value, double cos_value) {
    printf("%.4f\t\t%.4f\t\t%.4f\n", x, sin_value, cos_value);
}

// --- PART II. MAIN EXECUTER FUNCTION --- //
int main(void) {

    //Initializes the necessary variables for computation and printing
    double x; 
    double sin_value;
    double cos_value;


    // PRINTS OUT ALL THE SINE AND COSINE VALUES AS TABLE
    printf("x(radians)\tsin(x)\t\tcos(x)\n");
    printf("----------------------------------------------\n");
    for(x = 0.01; x < 1.0; x += 0.01) {
        sin_value = calculate_sine(x);
        cos_value = calculate_cos(x);
        result(x, sin_value, cos_value);
    }
    return 0; // Exit safely
}