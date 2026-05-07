// This Program Computes the average weight for an unknown population of Elephant Seals
// by using Dynamic Arrays, Structs, and Encapsulated Functions.

#include <stdio.h>
#include <stdlib.h> 

// PART I. DATA STRUCTURES //

/* Define a custom package to hold all seal-related data together */
typedef struct {
    double *weights;  // The dynamic array
    int count;        // How many seals are currently in the array
    int capacity;     // How much total room the array currently has
} SealPopulation;


// PART II. CORE SUPPORTING FUNCTIONS //

/* 1. Encapsulated Function to Read File and Return a Populated Struct */
SealPopulation read_seal_data(const char *filename) {
    // Create an instance of our struct and set starting values
    SealPopulation pop;
    pop.count = 0;
    pop.capacity = 10; 
    
    // Allocate initial memory for the array inside the struct
    pop.weights = malloc(pop.capacity * sizeof(double));
    
    if (pop.weights == NULL) {
        printf("Error: Memory allocation failed.\n");
        return pop; // Returns the struct with a NULL pointer
    }

    // Open the file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file '%s'.\n", filename);
        free(pop.weights); 
        pop.weights = NULL; // Mark as failed
        return pop; 
    }

    double temp_weight;
    // Read and expand dynamically
    while (fscanf(file, "%lf", &temp_weight) == 1) {
        
        // If the array is full, stretch it
        if (pop.count == pop.capacity) {
            pop.capacity = pop.capacity * 2; 
            pop.weights = realloc(pop.weights, pop.capacity * sizeof(double));
            
            if (pop.weights == NULL) {
                printf("Error: Memory reallocation failed.\n");
                fclose(file);
                return pop;
            }
        }
        
        // Store the weight and increment the count
        pop.weights[pop.count] = temp_weight;
        pop.count++; 
    }

    fclose(file);
    return pop; // Return the entire bundled package
}

/* 2. Function to Compute Average Formula using the Struct */
double average_list(SealPopulation pop) {
    if (pop.count == 0) return 0.0; 

    double sum = 0.0;
    for(int i = 0; i < pop.count; i++) {
        sum += pop.weights[i]; 
    }
    return sum / pop.count;
}


// PART III. EXECUTER FUNCTION //
int main(void) {
    
    // 1. Get the bundled data from our reading function
    SealPopulation seals = read_seal_data("elephant_seal_data.txt");
    
    // Check if the reading process failed
    if (seals.weights == NULL) {
        return 1; 
    }

    // 2. Do the math (pass the whole struct into the function)
    double final_average = average_list(seals);
    
    // 3. Print the result
    printf("\nPopulation Mean (Average) of the Elephant Seal Population is: %.2lf\n", final_average);
    
    // 4. Clean up (free the memory inside the struct)
    free(seals.weights); 

    return 0; 
}