#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ==========================================
// DATA STRUCTURES
// ==========================================

// List of Departments
typedef enum {
    HR,
    Sales,
    Software,
    Research,
    Executive,
} Departments;

// Salary Data Struct
typedef struct {
    Departments dept_name;
    int Annual_Salary;
    unsigned Social_Security_Number;
} Employees;

// Function to actually print out the department names
const char* dept_strings[] = {"HR", "Sales", "Software", "Research", "Executive"};


// ==========================================
// HELPER FUNCTIONS
// ==========================================

/**
 * Random Base Salary Generator to make it more realistic.
 * @param max_variance The ceiling for the random number generation.
 * @return An integer representing the base salary.
 */
int generateBaseSalary(int max_variance) {
    // 10000 is the assumed hard minimum wage.
    // rand() % max_variance adds a random amount on top of that minimum.
    return 10000 + (rand() % max_variance);
}

/**
 * Evaluates the employee's department and calculates their total annual salary.
 * @param dept The enum value of the employee's department.
 * @return An integer representing the final total salary.
 */
int calculateSalary(Departments dept) {
    int total_salary = 0;

    switch (dept) {
        case HR:
            // Base between 10k and ~30k, plus up to 5k offset
            total_salary = generateBaseSalary(20000) + (rand() % 5000);
            break;

        case Sales:
            // Base between 10k and ~40k, plus up to 15k offset
            total_salary = generateBaseSalary(30000) + (rand() % 15000);
            break;

        case Software:
            // Base between 10k and ~80k, plus up to 10k offset
            total_salary = generateBaseSalary(70000) + (rand() % 10000);
            break;

        case Research:
            // Base between 10k and ~60k, plus up to 8k offset
            total_salary = generateBaseSalary(50000) + (rand() % 8000);
            break;

        case Executive:
            // Base between 10k and ~110k, plus up to 50k offset
            total_salary = generateBaseSalary(100000) + (rand() % 50000);
            break;

        default:
            total_salary = 0;
    }
    
    return total_salary;
}


// ==========================================
// MAIN FUNCTION
// ==========================================

int main(void){
    srand(time(NULL));

    Employees worker[10];

    for(int i = 0; i < 10; i++) {
        worker[i].Social_Security_Number = 100000000 + (rand() % 10000000); // acts like a real 9-digit life social number
        worker[i].dept_name = rand() % 5;
        
        // Used to be a whole switch statement in the main function, now it is separately encapsulated
        worker[i].Annual_Salary = calculateSalary(worker[i].dept_name);

        printf("\nEmployee SSN: %u has $%d of Annual Salary and is working in %s.", 
            worker[i].Social_Security_Number, 
            worker[i].Annual_Salary, 
            dept_strings[worker[i].dept_name]);
    }

    // Add a final newline for clean console output
    printf("\n");

    return 0; 
}