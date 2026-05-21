/**
 * @file MonteCarloDijkstraHW.cpp
 * @author Troy Celdran
 * @brief Graph Representation and Dijkstra's Shortest Path Finder
 * @date May 20, 2026
 * 
 * @section DESCRIPTION
 * This program implements an adjacency matrix-based graph network and Dijkstra's 
 * algorithm for point-to-point path planning. It models physical waypoints, 
 * directional constraints, and path weights as an interconnected node mesh.
 * 
 * @section BACKGROUND
 * Author Background: Junior Electrical Engineering Student
 * 
 * @section MOTIVATION
 * Developed to master high-level software state-tracking, data encapsulation, 
 * and structural memory allocation in C++. The core objective is learning to 
 * bridge low-level hardware control loops with high-level autonomous navigation, 
 * optimizing route efficiency and resource distribution for Robotics applications.
 * 
 * @section REFLECTION What I Learned (200+ Word Essay)
 * 
 * @subsection OVERVIEW 1. Core Focus & Methodology
 * This homework focused on implementing Dijkstra's Algorithm to find the shortest path 
 * from a starting node to a destination node. It utilized a randomly generated graph 
 * with varying paths, nodes, and weights through a Monte Carlo Simulation to map 
 * possible outcomes given an initial state.
 * 
 * @subsection BRIDGE 2. Bridging the EE-to-DSA Gap
 * As an EE major, having no standard Data Structures and Algorithms (DSA) background 
 * gave me a steeper learning curve when trying to understand the abstract concepts of 
 * Graph Theory. To bridge this gap, I connected these software theories to my background 
 * in Robotics, Power System networks, Embedded Systems, and Numerical Analysis.
 * 
 * @subsection ANALOGY 3. The Robotics & Hardware Analogy
 * I approached the combination of Monte Carlo Simulation and Dijkstra's Algorithm as a 
 * robot analyzing all possible physical paths, using its internal sensors and control loops 
 * to find the most efficient route from Node 0 to Node n. Dijkstra's Algorithm acts like a 
 * careful explorer: it examines all adjacent paths, evaluates the distances, and continually 
 * updates its memory registers to lock in the optimal route.
 * 
 * @subsection INSIGHTS 4. Virtualization & Robustness
 * Furthermore, building this in C++ taught me how to translate physical hardware networks 
 * into virtual memory structures. Tracking node states with the boolean visited array felt 
 * exactly like managing hardware registers, and I learned to handle structural memory 
 * allocation using adjacency matrices. Ultimately, the Monte Carlo Simulation proved the 
 * algorithm's robustness by feeding it random "city grids," ensuring the navigation logic 
 * works flawlessly before a robot physically proceeds to travel.
 */

// IMPLEMENTATION CHOICE: 
// I chose to use a Connectivity Matrix (Adjacency Matrix) rather than an Edge List. 
// Because the Monte Carlo simulation tests edge densities of 20% and 40%, the graph 
// is relatively dense. The matrix allows O(1) constant-time lookups to check if an 
// edge exists between two nodes, which optimizes the inner loop of Dijkstra's algorithm.
#include <iostream>
#include <vector>
#include <limits>
#include <random>

// Class to represent the network hardware (Adjacency Matrix)
class MyGraph {
    private:
    int num_vertices;
    std::vector<std::vector<double>> graph_matrix;

    public:
    MyGraph(int vertices) {
        num_vertices = vertices;
        graph_matrix = std::vector<std::vector<double>>(
        num_vertices,
        std::vector<double>(num_vertices, std::numeric_limits<double>::infinity())
    );

    for (int n = 0; n < num_vertices; n++) {
        graph_matrix[n][n] = 0.0;
        }
    }

    int get_num_vertices() const {
        return num_vertices;
    }

    // Establishes a bidirectional physical connection between two nodes
    void connect_edge(int i, int j, double cost){
        //If statement to ensure connect_edge function not "overreaching"
        if (i < 0 || i >= num_vertices || j < 0 || j >= num_vertices) {
        throw std::out_of_range("Node index out of range");
    }
        graph_matrix[i][j] = cost;
        graph_matrix[j][i] = cost;
    }

    // Acts as a sensor pin to read the distance value between nodes
    double get_edge_weight(int i, int j) const {
        return graph_matrix[i][j];
    }

    // BONUS FUNCTION: I WANTED TO EXPORT AND SIMULATE THE ACTUAL GRAPH IN GRAPHVIZ ONLINE
    void save_to_dot_file() {
    std::cout << "copy this into graphviz online:" << std::endl;
    std::cout << "graph G {" << std::endl;
    for (int i = 0; i < num_vertices; i++) {
        for (int j = i + 1; j < num_vertices; j++) {
            if (graph_matrix[i][j] != std::numeric_limits<double>::infinity()) {
                std::cout << "  " << i << " -- " << j 
                        << " [label=\"" << (int)graph_matrix[i][j] << "\"];" << std::endl;
            }
        }
    }
    std::cout << "}" << std::endl;
}
}; 


// Class implementing Dijkstra's pathfinding controller
class ShortestPath {
    private:
    // Helper function: acts like a hardware comparator to find the lowest voltage/distance
    int get_min_node(const std::vector<double>& distances, const std::vector<bool>& visited, int n){

        double min_value = std::numeric_limits<double>::infinity();
        int min_index = -1; 

        for (int i = 0; i < n; i++){
            if((visited[i] == false) && (distances[i] <= min_value)){
                min_value = distances[i];
                min_index = i;
            }
        }
        return min_index;
    }


    public:
    // Calculates the shortest path between a starting node and a target node
    double path_size(MyGraph& map, int start_node, int target_node) {
    int n = map.get_num_vertices();
    
    // Memory registers to track the minimum distances and visited node states
    std::vector<double> distances(n, std::numeric_limits<double>::infinity());
    std::vector<bool> visited(n, false);
    
    // Initial state: distance to the starting node is always zero
    distances[start_node] = 0.0;

    for(int count = 0; count < n; count++){
        int current = get_min_node(distances, visited, n);

        // Safety breaker: stops the loop if remaining nodes are unreachable (open circuit)
        if (current == -1) break;
        
        // Latch the node state to prevent re-evaluation
        visited[current] = true;

        // Edge relaxation: updates neighboring nodes if a more efficient route is found
        for(int v = 0; v < n; ++v){
                if((visited[v] == false) && (map.get_edge_weight(current, v) != std::numeric_limits<double>::infinity()) && (distances[current] + map.get_edge_weight(current, v) < distances[v])){
                    distances[v] = distances[current] + map.get_edge_weight(current, v);
                }
            }
        }
    return distances[target_node];
    }

};

// External helper function to generate a random graph for Monte Carlo simulations
void generate_random_edges(MyGraph& graph, double density, double min_distance, double max_distance, std::mt19937& rng) {
    int n = graph.get_num_vertices();
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);          // moved outside
    std::uniform_real_distribution<double> dist_dist(min_distance, max_distance);  // moved outside

    for (int e = 0; e < n; e++) {
        for (int f = e + 1; f < n; f++) {
            if (prob_dist(rng) < density) {
                graph.connect_edge(e, f, dist_dist(rng));
            }
        }
    }
}

// Main execution routine: Monte Carlo Simulation
int main() {
    // Seed the random number generator
    std::mt19937 rng(std::random_device{}());

    int total_nodes = 50;
    double densities[] = {0.20, 0.40}; 

    // Loop through target edge densities (20% and 40%)
    for (double density : densities) {
        
        // Initialize a fresh 50-node hardware map
        MyGraph simulation_map(total_nodes);
        
        // Populate the map with random connections
        generate_random_edges(simulation_map, density, 1.0, 10.0, rng);

        // Boot up the navigation controller
        ShortestPath navigator;
        
        double path_sum = 0.0;
        int successful_paths = 0;

        // Calculate average path lengths from Node 0 to all other 49 nodes
        for (int target = 1; target < total_nodes; target++) {
            double distance = navigator.path_size(simulation_map, 0, target);
            
            if (distance != std::numeric_limits<double>::infinity()) {
                path_sum += distance;
                successful_paths++;
            }
        }

        double average = (successful_paths > 0) ? (path_sum / successful_paths) : 0.0;

        // 1. Print standard simulation telemetry to the terminal
        std::cout << "==========================================" << std::endl;
        std::cout << "Density Results: " << (density * 100) << "%:" << std::endl;
        std::cout << "  Identified Paths: " << successful_paths << "/49" << std::endl;
        std::cout << "  Average Shortest Path Length: " << average << std::endl;
        std::cout << "==========================================" << std::endl;

        // 2. Output the distinct Graphviz data block
        std::cout << "\n--- START GRAPHVIZ DATA [" << (density * 100) << "% DENSITY] ---" << std::endl;
        simulation_map.save_to_dot_file();
        std::cout << "--- END GRAPHVIZ DATA [" << (density * 100) << "% DENSITY] ---\n" << std::endl;
    }

    return 0;
}

