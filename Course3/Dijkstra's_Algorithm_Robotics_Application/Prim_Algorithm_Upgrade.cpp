/**
 * @file Prim_Algorithm_Upgrade.cpp
 * @author Troy Celdran
 * @brief Graph Representation, Dijkstra's Shortest Path + Prim's MST (HW3 Upgrade)
 *
 * @section DESCRIPTION
 * Adjacency matrix-based weighted undirected graph with Dijkstra's shortest path
 * and Monte Carlo simulation. Extended in HW3 to support Prim's MST algorithm
 * and file-based graph loading.
 *
 * @section MOTIVATION
 * Bridges low-level hardware control loops with high-level autonomous navigation
 * in C++ — optimizing route efficiency for Robotics applications.
 *
 * @section HARDWARE & ROBOTICS ANALOGY
 * - Dijkstra's = A robot explorer updating internal registers with the cheapest
 * known route from a starting node at each step — similar to sensor-driven
 * point-to-point path planning.
 * - Prim's MST = Deploying a unified communication grid across all robot bases
 * using the absolute minimum length of wire.
 * - Both rely on the same hardware comparator logic (get_min_node) to select
 * the next optimal connection.
 *
 * @section HW3 UPGRADE OVERVIEW (Connection to HW2 Program)
 * 1. Data Input: Transitioned from the Monte Carlo random graph generator 
 * to a deterministic file reader constructor. This allows testing against 
 * standardized network grids (e.g., parsing triplets from mst.txt).
 * 2. Logic Reuse: Reused the core greedy selection loop (`get_min_node`) 
 * from the Dijkstra implementation to drive Prim's algorithm.
 * 3. Objective Shift: Dijkstra accumulates historical path distances 
 * (`distances[current] + edge`), whereas the new Prim's implementation 
 * only evaluates the immediate cheapest connection (`key[v] = edge`) 
 * to span the entire graph without forming cycles.
 */

// NOTE: Adjacency Matrix chosen over Edge List for O(1) edge lookups,
// which optimizes both Dijkstra's inner loop and Prim's MST.
// Trade-off: O(V²) memory — fine for V=50, but use adjacency list for large sparse graphs.

#include <iostream>
#include <vector>
#include <limits>    
#include <random>    
#include <fstream>   
#include <stdexcept> 
#include <algorithm>
#include <sstream>

// -----------------------------------------------------------------------------
// CLASS: MyGraph — Weighted undirected graph via V×V adjacency matrix
// ∞ = no edge | 0.0 on diagonal = self-loop cost
// EE Analogy: Bus impedance matrix — ∞ means electrically isolated nodes
// -----------------------------------------------------------------------------
class MyGraph {
    private:
        int num_vertices;
        std::vector<std::vector<double>> graph_matrix; 

    public:
        int get_num_vertices() const {
            return num_vertices;
        }

        void connect_edge(int i, int j, double cost) {
            if (i < 0 || i >= num_vertices || j < 0 || j >= num_vertices) {
                throw std::out_of_range("Node index out of range");
            }
            graph_matrix[i][j] = cost;
            graph_matrix[j][i] = cost;
        }

        double get_edge_weight(int i, int j) const {
            return graph_matrix[i][j];
        }

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

        // UPGRADE: File reader constructor parsing text files into the matrix
        MyGraph(const std::string& filename) {
            std::ifstream file(filename);
            int v;
            file >> v;

            num_vertices = v;
            graph_matrix = std::vector<std::vector<double>>(
                num_vertices,
                std::vector<double>(num_vertices, std::numeric_limits<double>::infinity())
            );
            for (int n = 0; n < num_vertices; n++) {
                graph_matrix[n][n] = 0.0;
            }

            int i, j;
            double cost;
            while (file >> i >> j >> cost) {
                connect_edge(i, j, cost);  
            }
        }

        void save_to_dot_file() {
            std::cout << "Copy into Graphviz Online (https://dreampuf.github.io/GraphvizOnline/):" << std::endl;
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

// -----------------------------------------------------------------------------
// CLASS: ShortestPath — Dijkstra's Algorithm and Prim's MST
// -----------------------------------------------------------------------------
class ShortestPath {
    private:
        int get_min_node(const std::vector<double>& distances, const std::vector<bool>& visited, int n) {
            double min_value = std::numeric_limits<double>::infinity();
            int min_index = -1;
            for (int i = 0; i < n; i++) {
                if ((visited[i] == false) && (distances[i] < min_value)) {
                    min_value = distances[i];
                    min_index = i;
                }
            }
            return min_index;
        }

    public:
        // PREVIOUS IMPLEMENTATION: Point-to-Point shortest path
        double path_size(MyGraph& map, int start_node, int target_node) {
            int n = map.get_num_vertices();
            std::vector<double> distances(n, std::numeric_limits<double>::infinity());
            std::vector<bool> visited(n, false);

            distances[start_node] = 0.0; 

            for (int count = 0; count < n; count++) {
                int current = get_min_node(distances, visited, n);
                if (current == -1) break; 

                visited[current] = true; 

                for (int v = 0; v < n; ++v) {
                    double edge = map.get_edge_weight(current, v);
                    if ((visited[v] == false) &&
                        (edge != std::numeric_limits<double>::infinity()) &&
                        (distances[current] + edge < distances[v])) {
                        distances[v] = distances[current] + edge;
                    }
                }
            }
            return distances[target_node];
        }

        // UPGRADE: Minimum Spanning Tree across the entire graph network
        void get_MST(MyGraph& map) {
            int n = map.get_num_vertices();
            
            std::vector<double> key(n, std::numeric_limits<double>::infinity()); 
            std::vector<bool> inMST(n, false);   
            std::vector<int> parent(n, -1);      

            key[0] = 0.0; 

            for (int count = 0; count < n; count++) {
                int u = get_min_node(key, inMST, n); 
                if (u == -1) break; 
                
                inMST[u] = true;

                for (int v = 0; v < n; v++) {
                    double edge = map.get_edge_weight(u, v);
                    if (!inMST[v] && edge != std::numeric_limits<double>::infinity() && edge < key[v]) {                                        
                        key[v] = edge;
                        parent[v] = u;
                    }
                }
            }
            
            double total_cost = 0.0;
            
            std::cout << "\n==========================================" << std::endl;
            std::cout << "MST Edges (Parent -- Node == Cost):" << std::endl;
            std::cout << "==========================================" << std::endl;

            for (int i = 1; i < n; ++i) {
                if (parent[i] != -1) { 
                    total_cost += key[i]; 
                    std::cout << "  " << parent[i] << " -- " << i << " == " << key[i] << std::endl;
                }
            }

            std::cout << "------------------------------------------" << std::endl;
            std::cout << "Total MST Cost: " << total_cost << std::endl;
            std::cout << "==========================================\n" << std::endl;
        } 
}; 

// PREVIOUS IMPLEMENTATION: Monte Carlo network generation
void generate_random_edges(MyGraph& graph, double density,
                        double min_distance, double max_distance,
                        std::mt19937& rng) {
    int n = graph.get_num_vertices();
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
    std::uniform_real_distribution<double> dist_dist(min_distance, max_distance);

    for (int e = 0; e < n; e++) {
        for (int f = e + 1; f < n; f++) {
            if (prob_dist(rng) < density) {
                graph.connect_edge(e, f, dist_dist(rng));
            }
        }
    }
}
// -----------------------------------------------------------------------------
// MAIN FUNCTION
// -----------------------------------------------------------------------------
int main() {
    // Executes the HW3 Upgrade: File I/O + Prim's MST
    MyGraph file_graph("mst.txt");
    ShortestPath solver;
    solver.get_MST(file_graph);

    return 0;
}