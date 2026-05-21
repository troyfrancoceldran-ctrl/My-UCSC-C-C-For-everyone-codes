/**
 * @file Robot_ShortestPath_Dijkstra.cpp
 * @author Troy Celdran
 * @brief Graph Representation and Dijkstra's Shortest Path Finder
 * @date May 20, 2026
 *
 * @section DESCRIPTION
 * This program implements an adjacency matrix-based undirected weighted graph
 * and Dijkstra's algorithm for point-to-point shortest path planning.
 *
 * It models physical waypoints as integer-indexed nodes, bidirectional edges
 * as symmetric matrix entries, and traversal costs as double-precision weights.
 * The algorithm greedily selects the lowest-cost unvisited node at each step,
 * relaxing neighbor distances until the target node is settled.
 *
 * @section BACKGROUND
 * Author Background: Junior Electrical Engineering Student
 *
 * @section MOTIVATION
 * Developed to master high-level software state-tracking, data encapsulation,
 * and structural memory allocation in C++. The core objective is learning to
 * bridge low-level hardware control loops with high-level autonomous navigation,
 * optimizing route efficiency and resource distribution for robotics applications.
 *
 * @section COMPLEXITY
 * - Time:  O(V^2) — linear scan for minimum node over V vertices
 * - Space: O(V^2) — full adjacency matrix storage
 *
 * @section USAGE
 * Compile: g++ -std=c++17 -o dijkstra Shortest-path-Dijkstra.cpp
 * Run:     ./dijkstra
 */

// IMPLEMENTATION CHOICE: 
// I chose to use a Connectivity Matrix (Adjacency Matrix) rather than an Edge List. 
// For small, bounded embedded robotics waypoint maps, the matrix format provides 
// deterministic O(1) constant-time lookups to inspect connection states, bypassing 
// the pointer overhead or dynamic resizing costs associated with dynamic edge lists.

#include <iostream>
#include <vector>
#include <limits>
#include <cstdlib>
#include <ctime>

using namespace std;


/**
 * @class MyGraph
 * @brief Undirected weighted graph backed by an adjacency matrix.
 *
 * Nodes are identified by zero-based integer indices [0, num_vertices).
 * Edge weights are stored as doubles; missing edges are represented by
 * positive infinity. Self-loops are initialized to 0.
 *
 * @note This implementation assumes an undirected graph: connect_edge(i, j, w)
 * writes the same weight to both [i][j] and [j][i].
 */
class MyGraph {
    private:

    int num_vertices;                             ///< Total number of nodes in the graph
    std::vector<std::vector<double>> graph_matrix; ///< Adjacency matrix (num_vertices x num_vertices)

    public:

    /**
     * @brief Constructs an empty graph with a fixed number of vertices.
     *
     * Allocates a (vertices x vertices) matrix initialized to +infinity,
     * then sets the diagonal to 0 (zero-cost self-loops).
     *
     * @param vertices  Number of nodes; must be >= 1.
     */
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

    /**
     * @brief Returns the total number of vertices in the graph.
     * @return Integer node count.
     */
    int get_num_vertices() {
        return num_vertices;
    }

    /**
     * @brief Adds a bidirectional (undirected) weighted edge between two nodes.
     *
     * Writes @p cost to both [i][j] and [j][i] in the adjacency matrix.
     * Calling this a second time on the same pair overwrites the previous weight.
     *
     * @param i     Source node index.
     * @param j     Destination node index.
     * @param cost  Non-negative traversal cost for this edge.
     *
     * @warning Passing negative costs will not cause a crash but will
     * produce incorrect results in Dijkstra's algorithm, which
     * assumes all edge weights are >= 0.
     */
    void connect_edge(int i, int j, double cost) {
        graph_matrix[i][j] = cost;
        graph_matrix[j][i] = cost;
    }

    /**
     * @brief Returns the weight of the edge between two nodes.
     *
     * Returns +infinity if no edge has been set between @p i and @p j.
     *
     * @param i  Source node index.
     * @param j  Destination node index.
     * @return   Edge weight as a double, or +infinity if unreachable.
     */
    double get_edge_weight(int i, int j) {
        return graph_matrix[i][j];
    }

    /**
     * @brief Exports internal graph matrix paths into a formatted Graphviz text stream.
     *
     * Outputs data utilizing the Graphviz DOT language format. Can be pasted directly
     * into compatible layout engines to generate structural layout diagrams of the network topology.
     */
    void save_to_dot_file() {
        std::cout << "=== COPY EVERYTHING BELOW THIS LINE TO GRAPHVIZ ===" << std::endl;
        std::cout << "graph G {" << std::endl;
        std::cout << "  layout=neato;" << std::endl; // Keeps spatial layouts clean and proportional
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

}; // class MyGraph


/**
 * @class ShortestPath
 * @brief Computes shortest paths in a weighted graph using Dijkstra's algorithm.
 *
 * Uses an O(V^2) greedy approach with a linear minimum scan. Suitable for
 * dense graphs or small node counts typical in embedded robotics waypoint maps.
 *
 * @note For sparse graphs or large V, a priority-queue variant (O((V + E) log V))
 * would be more efficient.
 */
class ShortestPath {
    private:

    /**
     * @brief Scans the distance table for the lowest-cost unvisited node.
     *
     * Iterates over all n nodes, skipping those already marked visited,
     * and returns the index of the node with the minimum tentative distance.
     * Returns -1 if every node has been visited or all remaining distances
     * are +infinity (disconnected graph).
     *
     * @param distances  Tentative shortest distances from the source node.
     * @param visited    Boolean flags; true means the node is finalized.
     * @param n          Total number of nodes.
     * @return           Index of the unvisited node with the smallest distance,
     * or -1 if none exists.
     */
    int get_min_node(const std::vector<double>& distances,
                    const std::vector<bool>&   visited,
                    int n) {

        double min_value = std::numeric_limits<double>::infinity();
        int    min_index = -1;

        for (int i = 0; i < n; i++) {
            if (!visited[i] && distances[i] <= min_value) {
                min_value = distances[i];
                min_index = i;
            }
        }
        return min_index;
    }


    public:

    /**
     * @brief Returns the cost of the shortest path between two nodes.
     *
     * Runs Dijkstra's algorithm on @p map starting from @p start_node.
     * At each iteration the nearest unvisited node is finalized, and its
     * neighbors' tentative distances are relaxed if a cheaper route is found.
     * The loop runs exactly V times, guaranteeing all reachable nodes are settled.
     *
     * @param map          Reference to the graph to search.
     * @param start_node   Zero-based index of the origin node.
     * @param target_node  Zero-based index of the destination node.
     * @return             Minimum cumulative edge-weight from start to target,
     * or +infinity if no path exists.
     *
     * @note Does not reconstruct the node sequence of the path — only the cost.
     * To recover the actual route, a predecessor array would be needed.
     */
    double path_size(MyGraph& map, int start_node, int target_node) {
        int n = map.get_num_vertices();

        // distances[v] = best known cost to reach node v from start_node
        std::vector<double> distances(n, std::numeric_limits<double>::infinity());

        // visited[v] = true once node v's shortest distance is finalized
        std::vector<bool> visited(n, false);

        // Source node costs nothing to reach from itself
        distances[start_node] = 0.0;

        for (int count = 0; count < n; count++) {

            // Pick the cheapest unvisited node and finalize it
            int current = get_min_node(distances, visited, n);

            // Guard: graph may be disconnected — no reachable nodes remain
            if (current == -1) break;

            visited[current] = true;

            // Relax edges: update neighbors if a shorter route is found
            for (int v = 0; v < n; ++v) {
                double edge = map.get_edge_weight(current, v);

                if (!visited[v]
                    && edge != std::numeric_limits<double>::infinity()
                    && distances[current] + edge < distances[v]) {

                    distances[v] = distances[current] + edge;
                }
            }
        }

        return distances[target_node];
    }

}; // class ShortestPath


/**
 * @brief Program entry point — builds a sample robot waypoint map,
 * queries the shortest path between two nodes, and outputs visualization data.
 *
 * Graph topology (hand-drawn reference):
 *
 * @code
 * 9
 * 0 -------- 3
 * |\         |
 * 4| \6      5|
 * |  \       |
 * 2   4 ---- (1)   <- target
 * \  (no direct edge from 4 to 1 in this map)
 * 5 \
 * 4
 * @endcode
 *
 * Edges:
 * 0--3 : 9.0
 * 3--1 : 5.0   (only path to node 1 passes through node 3)
 * 0--2 : 4.0
 * 0--4 : 6.0
 * 2--4 : 5.0
 *
 * Expected shortest path 0 -> 1 :  0 -> 3 -> 1  (cost = 14.0)
 *
 * @return 0 on successful execution.
 */
int main() {

    // 1. Initialize the waypoint map for the robot's environment (5 nodes)
    MyGraph robot_map(5);

    // 2. Wire up edges matching the hand-drawn graph blueprint
    robot_map.connect_edge(0, 3, 9.0);
    robot_map.connect_edge(3, 1, 5.0);
    robot_map.connect_edge(0, 2, 4.0);
    robot_map.connect_edge(0, 4, 6.0);
    robot_map.connect_edge(2, 4, 5.0);

    // 3. Instantiate the Dijkstra navigation controller
    ShortestPath navigator;

    // 4. Query the shortest path cost from Node 0 to Node 1
    int    start    = 0;
    int    target   = 1;
    double shortest_distance = navigator.path_size(robot_map, start, target);

    // 5. Output navigation telemetry to the terminal
    std::cout << "--- ROBOT NAVIGATION TELEMETRY ---" << std::endl;
    std::cout << "Start Node:              " << start             << std::endl;
    std::cout << "Target Node:             " << target            << std::endl;
    std::cout << "Shortest Path Distance:  " << shortest_distance << std::endl;
    std::cout << "----------------------------------\n"          << std::endl;

    // 6. Output the schematic structure to paste directly into Graphviz Online
    robot_map.save_to_dot_file();

    return 0;
}