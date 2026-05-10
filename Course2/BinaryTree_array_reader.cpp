/**
 * ============================================================================
 * @file        BinaryTree_array_reader.cpp
 * @author      Troy Celd
 * @date        May 9, 2026
 * @brief       File Data Ingestion and Binary Search Tree (BST) Traversal
 *
 * @description
 * This program reads a sequence of integers from an external file. The first
 * integer dictates the allocation size (N) for the subsequent data to be read. 
 * The data is read into a dynamic container and then sequentially inserted 
 * into a Binary Search Tree (BST). Finally, an inorder traversal is performed 
 * to output the tree's values to the console in sorted, ascending order.
 *
 * @design_notes
 * Designed with strict adherence to Modern C++ standards:
 * - RAII (Resource Acquisition Is Initialization): Employs std::unique_ptr 
 * for all tree node allocations, ensuring automatic, leak-free memory 
 * cleanup without manual 'delete' calls.
 * - Encapsulation: The Node structure and root pointer are strictly private 
 * mechanics of the BST class. The client interface is limited to clean 
 * public methods like insert() and printer().
 * - Container Safety: Avoids raw C-style arrays in favor of safely managed 
 * dynamic containers (std::vector) for initial data ingestion.
 * ============================================================================
 */

#include <fstream>
#include <iostream>      
#include <vector>
#include <memory> 

using namespace std;  

// ============================================================================
// CLASS: BinarySearchTree
// ============================================================================
class BinarySearchTree {
private:
    // Internal data structure representing a single tree link
    struct Node {
        int value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
    };

    // The starting point of the tree (automatically initialized to nullptr)
    std::unique_ptr<Node> root;

    /**
     * @brief Recursively finds the correct insertion point for a new value.
     * @param current A reference to the node pointer being evaluated.
     * @param value The integer value to insert.
     */
    void insertHelper(std::unique_ptr<Node>& current, int value) {
        // Base Case: Empty spot found. Allocate new node here and stop.
        if (!current) {
            current = std::make_unique<Node>();
            current->value = value;
            return; 
        }

        // Recursive Step: Route left if smaller, right if greater or equal
        if (value < current->value) {
            insertHelper(current->left, value);
        } else {
            insertHelper(current->right, value);
        }
    }

    /**
     * @brief Recursively performs an inorder traversal (Left, Root, Right).
     * @param current A const reference to the current node pointer.
     */
    void printer(const std::unique_ptr<Node>& current) const {
        if (!current) return; // Base case: stop at dead ends

        printer(current->left);              // 1. Traverse Left
        std::cout << current->value << " ";  // 2. Visit Node (Print)
        printer(current->right);             // 3. Traverse Right
    }
    
public:
    // Default constructor
    BinarySearchTree() {}

    /**
     * @brief Public interface to insert a new value into the BST.
     * @param value The integer to insert.
     */
    void insert(int value) {
        insertHelper(root, value); 
    }

    /**
     * @brief Public interface to print the BST in ascending order.
     */
    void printer() const {
        printer(root);
        std::cout << '\n'; // Flush console output with a clean newline
    }
};

// ============================================================================
// MAIN EXECUTION
// ============================================================================
int main() {          
    // 1. Initialize File I/O
    std::ifstream inputFile("numbers.txt");
    
    // Always verify the file stream opened successfully
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open numbers.txt" << '\n';
        return 1; 
    }

    // 2. Read the total number of elements to process
    int size;
    inputFile >> size;

    // 3. Prepare the BST and a dynamically sized array (vector) for the data
    BinarySearchTree myTree;          
    std::vector<int> data(size); 

    // 4. Ingest data and populate the tree simultaneously
    for(int i = 0; i < size; i++) {
        inputFile >> data[i];         // Read from file into vector
        myTree.insert(data[i]);       // Safely insert the value into the BST
    }

    // 5. Output the sorted results to the console
    myTree.printer();

    return 0;            
}