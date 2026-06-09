/**
 * @file HexGame_prototype1.cpp
 * @brief Implementation of the Hex Board game environment (Homework 4).
 * @details This program sets up an N x N Hex game board represented as a graph.
 * It handles COLORED ASCII rendering of the rhombus-shaped board in accordance
 * to the actual game, checks legal player moves, and efficiently checks for
 * winning conditions using Union-Find (Disjoint Set) for near O(1) amortized
 * win detection. An AI player via Monte Carlo simulation will be added in Homework 5.
 * 
 * @author Troy Celdran
 * @date June 8, 2026
 * @version Beta
 *
 * @note Style conforms to the project guidelines and Google C++ Style Guide.
 * Brace placement, naming conventions, encapsulation, and modularity are strictly maintained.
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <string>
#include <utility>

// ANSI escape codes for coloring Mac terminal output (R=Red, B=Blue pieces and borders) 
// NOTE: these codes work on Mac/Linux; Windows CMD may require additional setup.
namespace Color {
    const std::string RED    = "\033[31m";
    const std::string BLUE   = "\033[34m";
    const std::string RESET  = "\033[0m";
    const std::string BOLD   = "\033[1m";
    const std::string YELLOW = "\033[33m";
}

// Holds a row/column pair for a player's move input.
struct Coordinate {
    int row;
    int column;
};

enum class Player {NONE, RED, BLUE}; 

//----------------------------------------------------------

// CLASS # 1: The efficient Disjoint Set Game Engine
class DisjointSet {
private:
    mutable std::vector<int> parent;  // mutable allows path compression in const methods
    std::vector<int> size;

public:
    // Initialize N independent sets (0 to N-1)
    explicit DisjointSet(int N) {
        parent.resize(N);
        size.assign(N, 1);
        // Initially, every element is its own parent
        std::iota(parent.begin(), parent.end(), 0);
    }

    // Find the ultimate representative/root of element 'v'
    // Implements path compression for O(1) amortized lookups
    int find(int v) const {
        if (v == parent[v]) {
            return v;
        }
        return parent[v] = find(parent[v]);
    }

    // Returns true if elements 'i' and 'j' belong to the same connected set
    bool is_connected(int i, int j) const {
        return find(i) == find(j);
    }

    // Merges the sets containing elements 'i' and 'j'
    // Returns true if a merge happened, false if they were already in the same set
    bool merge(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);

        if (root_i == root_j) {
            return false;
        }

        // Union by size: attach the smaller tree under the larger tree
        if (size[root_i] < size[root_j]) {
            std::swap(root_i, root_j);
        }

        parent[root_j] = root_i;
        size[root_i] += size[root_j];
        return true;
    }
};

//----------------------------------------------------------

// CLASS # 2: Generates the Standard Colored Hex Board UI
class HexBoard {
private:
    int size;
    std::vector<std::vector<Player>> board;
    DisjointSet DSU;

    // Internal helper to verify if coordinates are within board bounds
    bool ValidBounds(int r, int c) const {
        return r >= 0 && r < size && c >= 0 && c < size;
    }

    // Returns all 6 valid neighbors surrounding a hexagonal coordinate
    std::vector<std::pair<int, int>> GetNeighbors(int r, int c) const {
        std::vector<std::pair<int, int>> neighbors;
        // Hex axial/offset coordinate adjacent variations
        int dr[] = {-1, -1, 0, 0, 1, 1};
        int dc[] = {0, 1, -1, 1, -1, 0};

        for (int i = 0; i < 6; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (ValidBounds(nr, nc)) {
                neighbors.push_back({nr, nc});
            }
        }
        return neighbors;
    }

    // Virtual boundary nodes for RED (top/bottom) and BLUE (left/right)
    int GetTopNode()    const {return size * size;}
    int GetBottomNode() const {return size * size + 1;}
    int GetLeftNode()   const {return size * size + 2;}
    int GetRightNode()  const {return size * size + 3;}

    // Converts a 2D board coordinate to a 1D DSU index
    int GetFlatBoard(int r, int c) const {
        return r * size + c;
    }

public:
    // Constructor initializes an empty board of size N x N
    explicit HexBoard(int boardSize)
        :   size(boardSize),
            board(boardSize, std::vector<Player>(boardSize, Player::NONE)),
            DSU(boardSize * boardSize + 4) {}

    // Places a piece on the board and updates DSU connections.
    // Returns false if the move is out of bounds or the cell is already occupied.
    bool PlacePiece(int r, int c, Player player) {
        if (!ValidBounds(r, c) || board[r][c] != Player::NONE || player == Player::NONE) {
            return false;
        }

        // 1. Place the piece on the board
        board[r][c] = player;

        // 2. Get the 1D flat ID for the DSU
        int current_id = GetFlatBoard(r, c);

        // 3. Unite with any adjacent cells belonging to the same player
        for (const auto& neighbor : GetNeighbors(r, c)) {
            int nr = neighbor.first;
            int nc = neighbor.second;
            if (board[nr][nc] == player) {
                DSU.merge(current_id, GetFlatBoard(nr, nc));
            }
        }

        // 4. Unite with virtual boundary nodes if the piece touches a board edge
        if (player == Player::RED) {
            if (r == 0)        DSU.merge(current_id, GetTopNode());
            if (r == size - 1) DSU.merge(current_id, GetBottomNode());
        } else if (player == Player::BLUE) {
            if (c == 0)        DSU.merge(current_id, GetLeftNode());
            if (c == size - 1) DSU.merge(current_id, GetRightNode());
        }

        return true;
    }

    // Checks if the specified player has formed a winning connected path
    bool CheckWin(Player player) const {
        if (player == Player::RED) {
            return DSU.is_connected(GetTopNode(), GetBottomNode());
        } else if (player == Player::BLUE) {
            return DSU.is_connected(GetLeftNode(), GetRightNode());
        }
        return false;
    }

    // Renders the board with ANSI colors: red top/bottom borders for RED's axis,
    // blue left/right borders for BLUE's axis, and colored R/B pieces.
    void DisplayBoard() const {
        // Top column labels in red (RED connects top to bottom)
        std::cout << std::string(5, ' ');
        for (int c = 0; c < size; ++c) {
            std::cout << Color::RED << Color::BOLD << c;
            if (c < 10) std::cout << "   ";
            else std::cout << "  ";
        }
        std::cout << Color::RESET << "\n";

        // Top RED border line
        std::cout << std::string(4, ' ') << Color::RED << Color::BOLD << "+";
        for (int c = 0; c < size; ++c) {
            std::cout << "---";
            if (c < size - 1) std::cout << "-";
        }
        std::cout << "+" << Color::RESET << "\n";

        for (int r = 0; r < size; ++r) {
            // Left BLUE border and row label (BLUE connects left to right)
            std::cout << Color::BLUE << Color::BOLD;
            if (r < 10) std::cout << r << " | " << Color::RESET;
            else        std::cout << r << "| "  << Color::RESET;

            // Indent to create the geometric hexagonal slant
            std::cout << std::string(r, ' ');

            // Print each cell with appropriate color
            for (int c = 0; c < size; ++c) {
                if (board[r][c] == Player::RED) {
                    std::cout << Color::RED  << Color::BOLD << "R" << Color::RESET;
                } else if (board[r][c] == Player::BLUE) {
                    std::cout << Color::BLUE << Color::BOLD << "B" << Color::RESET;
                } else {
                    std::cout << ".";
                }
                if (c < size - 1) std::cout << " - ";
            }

            // Right BLUE border
            std::cout << Color::BLUE << Color::BOLD << " |" << Color::RESET << "\n";

            // Print diagonal connectors between rows
            if (r < size - 1) {
                std::cout << std::string(3 + r + 1, ' ');
                for (int c = 0; c < size; ++c) {
                    std::cout << "\\ ";
                    if (c < size - 1) std::cout << "/ ";
                }
                std::cout << "\n";
            }
        }

        // Bottom RED border line
        std::cout << std::string(4 + size, ' ') << Color::RED << Color::BOLD << "+";
        for (int c = 0; c < size; ++c) {
            std::cout << "---";
            if (c < size - 1) std::cout << "-";
        }
        std::cout << "+" << Color::RESET << "\n";

        // Bottom column labels in red
        std::cout << std::string(5 + size, ' ');
        for (int c = 0; c < size; ++c) {
            std::cout << Color::RED << Color::BOLD << c;
            if (c < 10) std::cout << "   ";
            else        std::cout << "  ";
        }
        std::cout << Color::RESET << "\n";
    }

    // Encapsulates the complete interactive game session loop
    void PlayGame() {
        Coordinate move;
        Player currentPlayer = Player::BLUE; // BLUE starts per standard rules

        // Runs as long as neither player has formed a winning path
        while (!CheckWin(Player::RED) && !CheckWin(Player::BLUE)) {
            std::cout << "\n---------------------------------------\n";
            DisplayBoard();
            std::cout << "---------------------------------------\n";

            // Display the current player's name in their color
            std::string playerText  = (currentPlayer == Player::RED) ? "RED" : "BLUE";
            std::string playerColor = (currentPlayer == Player::RED) ? Color::RED : Color::BLUE;
            std::cout << "Player " << playerColor << Color::BOLD << playerText
                    << Color::RESET << ", enter your move (row, column), [0.."
                    << size - 1 << "]: ";

            std::cin >> move.row >> move.column;

            // Guard against non-numeric input 
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                std::cout << "\nInvalid input! Please enter two numbers in range [0.." << size - 1 << "].\n";
                continue;
            }

            // Attempt to place the piece; swap turns only on a legal move
            if (PlacePiece(move.row, move.column, currentPlayer)) {
                currentPlayer = (currentPlayer == Player::RED) ? Player::BLUE : Player::RED;
            } else {
                std::cout << "\nInvalid move! Please try a different spot. Valid range: [0.." << size - 1 << "].\n";
            }
        }

        // Display the post-game board layout and announce the winner
        std::cout << "\n--- POST-GAME BOARD LAYOUT ---\n";
        DisplayBoard();
        std::cout << "\n---------------------------------------\n";
        std::cout << " GAME OVER. Care to try again? \n";
        std::cout << "---------------------------------------\n";

        if (CheckWin(Player::RED)) {
            std::cout << Color::RED  << Color::BOLD << "RED just crossed vertically!\n"   << Color::RESET;
        } else {
            std::cout << Color::BLUE << Color::BOLD << "BLUE just crossed horizontally!\n" << Color::RESET;
        }
    }
};

// MAIN FUNCTION
int main() {
    // Initialize the standard 11 x 11 board and start the game(Adjust size number for a smaller/bigger board)
    HexBoard game(11);

    game.PlayGame(); //Initializes the Game

    return 0;
}