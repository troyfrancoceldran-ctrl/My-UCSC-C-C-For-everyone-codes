// This Program Computes the average weight for an unknown population of Elephant Seals
// by using Dynamic Arrays, Pointers, and Encapsulated Functions.

#include <stdio.h>
#include <stdlib.h> 

// PART I. CORE SUPPORTING FUNCTIONS //

/* 1. Encapsulated Function to Read File and Build Dynamic Array */
// We pass 'count' as a pointer (int *out_count) so this function can update main's variable.
double* read_seal_data(const char *filename, int *out_count) {
    int capacity = 10; 
    int count = 0;
    double temporary_weight; 
    
    // Allocate initial memory
    double *weights = malloc(capacity * sizeof(double));
    if (weights == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL; // Return NULL to indicate failure
    }

    // Open the file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file '%s'.\n", filename);
        free(weights); 
        return NULL; 
    }

    // Read and expand dynamically
    while (fscanf(file, "%lf", &temporary_weight) == 1) {
        if (count == capacity) {
            capacity = capacity * 2; 
            weights = realloc(weights, capacity * sizeof(double));
            
            if (weights == NULL) {
                printf("Error: Memory reallocation failed.\n");
                fclose(file);
                return NULL;
            }
        }
        weights[count] = temporary_weight;
        count++; 
    }

    fclose(file);
    
    // Send the final count back to main using the pointer
    *out_count = count; 
    
    return weights; // Return the fully populated array
}

/* 2. Function to Compute Average Formula */
double average_list(int count, double data[]){
    if (count == 0) return 0.0; 

    double sum = 0.0;
    for(int i = 0; i < count; i++){
        sum += data[i]; // Cleaner shorthand for sum = sum + data[i]
    }
    return sum / count;
}

// PART II. EXECUTER FUNCTION //
int main(void){
    int count = 0;
    
    // 1. All file opening, reading, and memory allocation now in one line
    double *weights = read_seal_data("elephant_seal_data.txt", &count);
    
    // Check if the reading process failed
    if (weights == NULL) {
        return 1; 
    }

    // 2. Do the math
    double final_average = average_list(count, weights);
    
    // 3. Print the result
    printf("\nPopulation Mean (Average) of the Elephant Seal Population is: %.2lf\n", final_average);
    
    // 4. Clean up everything
    free(weights); 

    return 0; 
}