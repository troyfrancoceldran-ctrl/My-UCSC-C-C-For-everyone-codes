/**
 * @file      Doubly_list.c
 * @author    Troy Celd
 * @date      May 8, 2026
 * @brief     Implementation of a Doubly Linked List with Sort and Sweep.
 * @details   This program generates a doubly linked list of 200 random 
 * integers (range 0-49). It demonstrates dynamic memory management 
 * and pointer manipulation by applying a Bubble Sort to the list 
 * and subsequently traversing it to safely remove adjacent duplicates.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief Doubly-Linked List Node structure.
 * @details Stores an integer payload and pointers to adjacent nodes.
 */
typedef struct node {
    int data;           /**< Integer payload */
    struct node *prev;  /**< Pointer to the previous node in the list */
    struct node *next;  /**< Pointer to the next node in the list */
} Node;

/**
 * @brief List Wrapper structure.
 * @details Maintains pointers to the start and end of the list for O(1) tail access.
 */
typedef struct list{
    Node *head;        
    Node *tail;        
} List;

/**
 * @brief Sorts the doubly linked list in ascending order.
 * @param list Pointer to the List wrapper struct to be sorted.
 * @note This implementation uses Bubble Sort, swapping data payloads 
 * rather than rewiring node pointers for stability and simplicity.
 */
void list_sorter(List *list){
    // Safety check: An empty list or a list with 1 item is already sorted.
    if(list->head == NULL || list->head->next == NULL){
        printf("List is already sorted!\n");
        return;
    }
    
    int swapped; // Acts as a boolean flag to track if a pass made changes
    Node *current;

    do {
        swapped = 0;
        current = list->head;

        // Traverse the list until the second-to-last node
        while(current->next != NULL){
            
            // Compare adjacent node payloads
            if (current->data > current->next->data) { 
                // Swap the integer values using a temporary variable
                int temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;

                // Flag that a swap occurred so the loop runs again
                swapped = 1;
            }

            // Advance the walker pointer
            current = current->next;
        }
        
    } while (swapped);
}

/**
 * @brief Appends a new node with the given data to the end of the list.
 * @param list Pointer to the List wrapper struct.
 * @param data The integer payload to store in the new node.
 */
void push_back(List *list, int data){
    // Allocate memory for the new node on the heap
    Node *new_node = (Node *)malloc(sizeof(Node)); 
    
    // Safety check to verify valid memory allocation
    if (new_node == NULL) {
        fprintf(stderr, "Invalid Memory Allocation!\n");
        exit(1); // Abort program if heap memory is exhausted
    } else {
        new_node->data = data;
        new_node->next = NULL;
    }
    
    // Case 1: List is currently empty
    if (list->head == NULL){
        new_node->prev = NULL;
        list->head = list->tail = new_node;
        return;
    }

    // Case 2: List contains at least one node
    if (list->head != NULL){
        new_node->prev = list->tail;
        list->tail->next = new_node;
        list->tail = new_node; // Update wrapper tail pointer
    }
}

/**
 * @brief Traverses and prints all elements in the list.
 * @param list Pointer to the List wrapper struct to be printed.
 */
void print_out(List *list){
    Node* current = list->head;
    
    // Walk through the list and print each payload with a trailing space
    while(current != NULL){
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

/**
 * @brief Removes adjacent duplicate values from a sorted list.
 * @param list Pointer to the List wrapper struct to be swept.
 * @note This function safely rewires next/prev pointers and frees 
 * the memory of the duplicated nodes to prevent memory leaks.
 */
void duplicate_remover(List *list){
    Node *current = list->head;
    
    // Ensure we don't dereference a NULL pointer while checking neighbors
    while(current != NULL && current->next != NULL){
        
        // If current data matches the next node's data, we found a duplicate
        if(current->data == current->next->data){
            Node *duplicate = current->next;
            
            // Bypass the duplicate going forward
            current->next = duplicate->next;

            // Rewire the backward pointer of the node AFTER the duplicate
            if(duplicate->next != NULL){
                duplicate->next->prev = current;
            } else {
                // If the duplicate was the last node, update the list's tail
                list->tail = current;
            }
            
            // Release the duplicate node's memory back to the heap
            free(duplicate);
            
        } else {
            // Only move forward if no deletion occurred, 
            // ensuring we catch multiple identical duplicates in a row (e.g., 2, 2, 2)
            current = current->next;
        } 
    }
}

/**
 * @brief Main execution function.
 * @return 0 upon successful execution.
 */
int main(void){
    // Seed the random number generator using the current system time
    srand(time(NULL));

    // Initialize an empty list
    List my_list = {NULL, NULL};

    // Generate 200 random integers between 0 and 49 and push them to the list
    for(int i = 0; i < 200; i++){
        push_back(&my_list, rand() % 50);
    }
    
    // Step 1: Print the randomly generated unsorted integers
    printf("\nTHE RANDOMLY GENERATED 200 UNSORTED INTEGERS:\n");
    print_out(&my_list);

    // Step 2: Sort the list and print the result
    list_sorter(&my_list);
    printf("\nTHE SORTED LIST OF THE INTEGERS:\n");
    print_out(&my_list);

    // Step 3: Sweep out adjacent duplicates and print the final clean list
    duplicate_remover(&my_list);
    printf("\nTHE SORTED LIST WITHOUT DUPLICATES:\n");
    print_out(&my_list);

    return 0;
}