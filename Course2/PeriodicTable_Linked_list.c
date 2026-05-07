/*
 * Author: Troy Celdran
 * Project: Periodic Table Linked List
 *
 * Description:
 *   This program builds a singly linked list of the first 10 elements
 *   of the periodic table. Each node in the list holds data for one
 *   element using a struct, and memory for each node is allocated
 *   dynamically at runtime using malloc().
 *
 * Struct Design:
 *   - Define a struct (e.g., Element) with three members:
 *       char name[]     — full element name (e.g., "Hydrogen")
 *       char symbol[]   — atomic symbol (e.g., "H")
 *       float weight    — atomic weight (e.g., 1.008)
 *   - Add a pointer to the same struct type as the "next" pointer
 *     so each node can link to the next one in the list.
 *
 * Linked List Logic:
 *   - Maintain a "head" pointer that always points to the first node.
 *   - For each new element entered, allocate a new node with malloc(),
 *     populate its data members, set its next pointer to NULL, then
 *     attach it to the end of the existing list (tail insertion).
 *   - Use a temporary traversal pointer to walk to the last node
 *     before attaching each new one.
 *
 * Input Loop:
 *   - Loop exactly 10 times (one per element).
 *   - Each iteration: prompt the user to enter the name, symbol,
 *     and atomic weight, read them in, allocate a new node, fill it,
 *     and append it to the list.
 *
 * Output / Display:
 *   - After all 10 nodes are inserted, traverse the list from head
 *     to NULL, printing each element's number (use a counter),
 *     name, symbol, and atomic weight in a clean, aligned format.
 *
 * Memory Cleanup:
 *   - After printing, traverse the list a final time, freeing each
 *     node with free() before moving to the next, to avoid memory leaks.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Data structure for the Elements
typedef struct Element {
    char Elem_name[20];
    char Elem_symbol[10];
    float atomic_weight;
    struct Element *next;   // pointer to the SAME struct type
} Element;


// Telling the compiler these functions exist before main() uses them
Element* buildList(void);
void printTable(Element *head);
void freeList(Element *head);


// -----THE PRIMARY BUILDER FUNCTIONS-----

//Runs 10 loops to create the list(The actual Builder Function)
Element* buildList(void) {
    Element *head = NULL;

    for (int i = 0; i < 10; i++) {
        Element *elm = (Element *)malloc(sizeof(Element));
        if (elm == NULL) {
            fprintf(stderr, "Invalid Memory Allocation!\n");
            exit(1); // exit() immediately stops the whole program
        } 
        
        printf("Enter element %d name, symbol, and atomic weight: ", i + 1); 
        scanf("%s", elm->Elem_name);
        scanf("%s", elm->Elem_symbol);
        scanf("%f", &elm->atomic_weight);

        elm->next = NULL;
        
        if (head == NULL) {
            head = elm;
        } else {
            Element *temporary = head;
            while (temporary->next != NULL) {
                temporary = temporary->next;
            }
            temporary->next = elm;
        }
    }
    return head; // Give the completed list back to main()
}

//Formats the output like a table
void printTable(Element *head) {
    // Print the Table Header
    printf("\n");
    printf("%-5s | %-15s | %-10s | %-15s\n", "No.", "Name", "Symbol", "Atomic Weight");
    printf("---------------------------------------------------------\n");

    // Print the Table Rows
    int c = 1;
    Element *current = head;
    while (current != NULL) {
        // %-5d reserves 5 spaces for the integer, %-15s reserves 15 spaces for strings, etc.
        printf("%-5d | %-15s | %-10s | %-15.3f\n", 
            c, current->Elem_name, current->Elem_symbol, current->atomic_weight);
        
        current = current->next; 
        c++;
    }
    printf("\n");
}

//Function to clear the memory allocations 
void freeList(Element *head) {
    Element *current = head;
    while (current != NULL) {
        Element *freespace = current;
        current = current->next;
        free(freespace);
    }
}

// --- Main Function ---
int main(void) {
    // 1. Build the list and get the head pointer
    Element *head = buildList();
    
    // 2. Print the formatted table
    printTable(head);
    
    // 3. Clean up the memory
    freeList(head);
    
    return 0;
}