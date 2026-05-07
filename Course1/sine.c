/**
 * This program calculates the sine of an input continuous value.
 * The input-value is default in radians between 0 and 1 (exclusive).
 */

#include <stdio.h>
#include <math.h>

// PART I. INSTRUCTION FUNCTIONS ---

// 1. INPUT PROMPT FUNCTION
int input(double *x) {
    printf("Enter a value strictly between 0 and 1 (0 < x < 1): ");
    if (scanf("%lf", x) != 1) {
        printf("Error: Invalid input. Please enter a valid numerical value\n");
        return 0; // 0 indicates failure
    }
    return 1; // 1 indicates success
}

// 2. BOUND VALIDATION
int validate(double x) {
    if (x > 0.0 && x < 1.0) {
        return 1; // 1 indicates the value is within bounds
    }
    printf("Error: Value must be strictly between 0 and 1 (0 < x < 1).\n");
    return 0; // 0 indicates out of bounds
}

// 3. COMPUTATION FUNCTION
double calculate_sine(double x) {
    return sin(x);
}

// 4. OUTPUT DISPLAY FUNCTION
void result(double x, double sin_value) {
    printf("The sine of %.4f radians is %.4f\n", x, sin_value);
}

// --- PART II. EXECUTER FUNCTION ---
int main(void) {
    double x; 
    double sin_value;

    // Step 1: Read the input and verify it is a valid floating-point number
    if (!input(&x)) {
        return 1; // Exit early with an error code
    }

    // Step 2: Verify the input is strictly within the allowed bounds
    if (!validate(x)) {
        return 1; // Exit with an error code during bound violations
    }

    // Step 3: Calculate the sine value
    sin_value = calculate_sine(x);

    // Step 4: Display the result
    result(x, sin_value);

    return 0; // Exit safely
}