/*******************************************************************************
 * Author:       Troy Celd
 * Date:         May 9, 2026
 * Description:  This program reads a file of integers to perform arithmetic 
 * on rational numbers. The first integer in the file determines 
 * the size of a dynamically allocated array. The remaining 
 * integers are read in pairs to populate a 'Cycle' struct, 
 * representing a fraction with a numerator and a denominator.
 * * The program implements four modular routines to add, subtract, 
 * multiply, and divide these rational numbers. Finally, it uses 
 * these routines to process the entire array, calculating and 
 * printing the total sum and the average of all the rationals 
 * in the file.
 * * Math Logic:   Given Cycle 1 (a/b) and Cycle 2 (c/d):
 * - Addition:       (a*d + b*c) / (b*d)
 * - Subtraction:    (a*d - b*c) / (b*d)
 * - Multiplication: (a*c) / (b*d)
 * - Division:       (a*d) / (b*c)
 * * Input:        A text file of space-separated integers (e.g., "2 9 11 4 5").
 * Output:       The mathematical sum and average of the parsed rationals.
 * * PRELIMINARIES: 
 * Since this code only provides one data structure, we map the algebra as:
 * - Let a = Cycle a.numerator
 * - Let b = Cycle a.denominator
 * - Let c = Cycle b.numerator
 * - Let d = Cycle b.denominator 
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

// Data structure for the rational number
typedef struct cycle {
    int numerator;
    int denominator;
} Cycle;

// Routine to add two cycles
Cycle add_cycle(Cycle a, Cycle b) {
    Cycle result;
    result.numerator = (a.numerator * b.denominator) + (b.numerator * a.denominator);
    result.denominator = (a.denominator * b.denominator); 
    return result;
}

// Routine to subtract two cycles
Cycle subtract_cycle(Cycle a, Cycle b) {
    Cycle result;
    result.numerator = (a.numerator * b.denominator) - (b.numerator * a.denominator);
    result.denominator = (a.denominator * b.denominator); 
    return result;
}

// Routine to multiply two cycles
Cycle multiply_cycle(Cycle a, Cycle b) {
    Cycle result;
    result.numerator = (a.numerator * b.numerator);
    result.denominator = (a.denominator * b.denominator); 
    return result;
}

// Routine to divide two cycles
Cycle divide_cycle(Cycle a, Cycle b) {
    Cycle result;
    result.numerator = (a.numerator * b.denominator);
    result.denominator = (a.denominator * b.numerator); 
    return result;
}   

// Routine to read the fractions from a file into a dynamically allocated array
Cycle* array_cycle_file_reader(const char *filename, int *count) {
    FILE *file_pointer = fopen(filename, "r");
    
    // Check if the file failed to open
    if (file_pointer == NULL) {
        printf("Error: Could not open file '%s'.\n", filename);
        return NULL; 
    }

    // If file pointer is valid, proceed to read
    printf("File '%s' opened successfully!\n", filename);

    // Read the total count of elements
    fscanf(file_pointer, "%d", count);
    
    // Dynamically allocate memory for the array
    Cycle *cycle_array = malloc(*count * sizeof(Cycle));

    // Populate the array with numerators and denominators
    for(int i = 0; i < *count; i++) {
        fscanf(file_pointer, "%d %d", &cycle_array[i].numerator, &cycle_array[i].denominator);
    }
    
    // Clean up and return
    fclose(file_pointer);        
    return cycle_array;
}

// Main executor function
int main(void) {
    int total_elements = 0;
    
    // 1. Call reader function to populate the array (Updated to "data.txt")
    Cycle *data_array = array_cycle_file_reader("data.txt", &total_elements);
    
    // 2. Safety check for file reading errors
    if (data_array == NULL) {
        printf("Data array not found. Exiting program.\n");
        return 1;
    }

    // 3. Setup the total_sum fraction (Initialized to 0/1)
    Cycle total_sum;
    total_sum.numerator = 0;
    total_sum.denominator = 1;
    
    // 4. Loop through the array and accumulate the sum
    for(int i = 0; i < total_elements; i++) {
        total_sum = add_cycle(total_sum, data_array[i]);
    }
    
    // 5. Setup the count as a fraction (total_elements/1) for division
    Cycle count_cycle;
    count_cycle.numerator = total_elements;
    count_cycle.denominator = 1;

    // 6. Calculate the average using the divide_cycle function
    Cycle average = divide_cycle(total_sum, count_cycle);

    // 7. Print the final results
    printf("\n--- Results ---\n");
    printf("Total Sum: %d/%d\n", total_sum.numerator, total_sum.denominator);
    printf("Average:   %d/%d\n", average.numerator, average.denominator);

    // 8. Free the dynamically allocated array to prevent memory leaks
    free(data_array);
    
    return 0;
}