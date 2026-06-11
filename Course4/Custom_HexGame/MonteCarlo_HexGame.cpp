/**
 * @file MonteCarlo_HexGame.cpp
 * @brief Implementation of the Hex Board game environment with Monte Carlo AI Player (Homework 5).
 * @details This program sets up an N x N Hex game board represented as a graph.
 * It handles COLORED ASCII rendering of the rhombus-shaped board in accordance
 * to the actual game, checks legal player moves, and efficiently checks for
 * winning conditions using Union-Find (Disjoint Set) for near O(1) amortized
 * win detection.
 * Updated in v1.0: Added Monte Carlo AI opponent via randomized game-tree
 * exploration. AI evaluates 1000 simulations per legal move.
 * Alpha-Beta pruning optimization planned for future version.
 *
 * @author Troy Celdran
 * @date June 11, 2026
 * @version 1.0
 *
 * @note Style conforms to the project guidelines and Google C++ Style Guide.
 * Brace placement, naming conventions, encapsulation, and modularity are strictly maintained.
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <string>
#include <utility>
#include <random>   // for Monte Carlo Simulations

// ANSI escape codes for coloring terminal output (R=Red, B=Blue pieces and borders)
// NOTE: these codes work on Mac/Linux; Windows CMD may require additional setup.
namespace Color {
    const std::string RED    = "\033[31m";
    const std::string BLUE   = "\033[34m";
    const std::string RESET  = "\033[0m";
    const std::string BOLD   = "\033[1m";
    const std::string YELLOW = "\033[33m";
}

/// @brief Holds a row/column pair for a player's move input.
struct Coordinate {
    int row;
    int column;
};

enum class Player { NONE, RED, BLUE };

//----------------------------------------------------------

// CLASS #1: The efficient Disjoint Set Game Engine
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

// CLASS #2: Generates the Standard Colored Hex Board UI
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
        // Hex axial/offset coordinate adjacent directions
        int dr[] = {-1, -1, 0, 0, 1, 1};
        int dc[] = { 0,  1,-1, 1,-1, 0};

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
    int GetTopNode()    const { return size * size; }
    int GetBottomNode() const { return size * size + 1; }
    int GetLeftNode()   const { return size * size + 2; }
    int GetRightNode()  const { return size * size + 3; }

    // Converts a 2D board coordinate to a 1D DSU index
    int GetFlatBoard(int r, int c) const {
        return r * size + c;
    }

public:
    // Constructor initializes an empty board of size N x N
    explicit HexBoard(int boardSize)
        : size(boardSize),
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
            else        std::cout << "  ";
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

    // Runs Monte Carlo simulation to select the AI's best move.
    // For each legal move, simulates 1000 random game completions
    // and returns the move with the highest win rate.
    Coordinate BestAIMoveGenerator(Player ai_color, std::mt19937& rng) {
        // Collect all currently empty cells on the board
        std::vector<Coordinate> empty_cells;
        for (int r = 0; r < size; ++r) {
            for (int c = 0; c < size; ++c) {
                if (board[r][c] == Player::NONE) {
                    empty_cells.push_back({r, c});
                }
            }
        }

        // Tracks the best candidate move and its associated win count
        int best_win_count = -1;
        Coordinate best_move = empty_cells[0];
        int number_of_trials = 1000;  // 1000 trials per guidelines

        // Opponent color is fixed for the entire evaluation — compute once
        Player opponent_color = (ai_color == Player::RED) ? Player::BLUE : Player::RED;

        // Evaluate each empty cell as a potential AI move
        for (Coordinate candidate : empty_cells) {
            int wins = 0;  // win counter for this candidate

            // Build list of remaining empty cells excluding the candidate
            std::vector<Coordinate> remaining_cells;
            for (Coordinate c : empty_cells) {
                if (c.row != candidate.row || c.column != candidate.column) {
                    remaining_cells.push_back(c);
                }
            }

            // Run Monte Carlo trials: randomly fill the board and check if AI wins
            for (int i = 0; i < number_of_trials; i++) {
                HexBoard simulation_board = *this;
                simulation_board.PlacePiece(candidate.row, candidate.column, ai_color);

                // Randomly shuffle remaining moves to simulate a full game
                std::vector<Coordinate> shuffled_cells = remaining_cells;
                std::shuffle(shuffled_cells.begin(), shuffled_cells.end(), rng);

                // Alternate turns starting from the opponent
                Player simulation_turn = opponent_color;
                for (Coordinate random_move : shuffled_cells) {
                    simulation_board.PlacePiece(random_move.row, random_move.column, simulation_turn);
                    simulation_turn = (simulation_turn == Player::RED) ? Player::BLUE : Player::RED;
                }

                // Count this trial as a win if the AI's color won
                if (simulation_board.CheckWin(ai_color)) {
                    wins++;
                }
            }

            // Update best move if this candidate has a higher win rate
            if (wins > best_win_count) {
                best_win_count = wins;
                best_move = candidate;
            }
        }
        return best_move;  // Return the move with the highest simulated win rate
    }

    // Encapsulates the complete interactive game session loop.
    // Handles color selection, human input, AI move generation, and win detection.
    void PlayGame(std::mt19937& rng) {
        Coordinate move;
        Player currentPlayer = Player::BLUE;  // BLUE always starts per standard Hex rules

        // Prompt the human player to choose their color
        Player human_player;
        Player AI_player;
        char color_choice;

        std::cout << "Pick your Color:\nBLUE\nor\nRED\n";
        std::cin >> color_choice;

        // Assign player colors based on choice; BLUE always moves first
        switch (color_choice) {
            case ('B'):
            case ('b'):
                human_player = Player::BLUE;
                AI_player    = Player::RED;
                std::cout << "You are playing as BLUE. You go first!\n";
                break;
            case ('R'):
            case ('r'):
                human_player = Player::RED;
                AI_player    = Player::BLUE;
                std::cout << "You are playing as RED. AI goes first!\n";
                break;
            default:
                std::cout << "Invalid color choice! Defaulting to BLUE...\n";
                human_player = Player::BLUE;
                AI_player    = Player::RED;
                break;
        }

        // Runs as long as neither player has formed a winning path
        while (!CheckWin(Player::RED) && !CheckWin(Player::BLUE)) {
            std::cout << "\n---------------------------------------\n";
            DisplayBoard();
            std::cout << "---------------------------------------\n";

            // Branch between human input and AI move generation
            if (currentPlayer == human_player) {
                std::cout << "Your turn! Enter move (row, column) [0.." << size - 1 << "]: ";
                std::cin >> move.row >> move.column;

                // Guard against non-numeric input corrupting the stream
                if (std::cin.fail()) {
                    std::cin.clear();
                    std::cin.ignore(1000, '\n');
                    std::cout << "\nInvalid input! Please enter two numbers in range [0.."
                            << size - 1 << "].\n";
                    continue;
                }

                // Place piece on valid move; prompt retry on illegal move
                if (PlacePiece(move.row, move.column, currentPlayer)) {
                    currentPlayer = (currentPlayer == Player::RED) ? Player::BLUE : Player::RED;
                } else {
                    std::cout << "Invalid move. Valid range: [0.." << size - 1 << "]. Try again.\n";
                }

            } else {
                // AI evaluates all legal moves via Monte Carlo and picks the best
                std::cout << "AI player is calculating a move...\n";
                Coordinate ai_move = BestAIMoveGenerator(AI_player, rng);
                PlacePiece(ai_move.row, ai_move.column, currentPlayer);
                currentPlayer = (currentPlayer == Player::RED) ? Player::BLUE : Player::RED;
            }
        }

        // Display the post-game board layout and announce the winner
        std::cout << "\n--- POST-GAME BOARD LAYOUT ---\n";
        DisplayBoard();
        std::cout << "\n---------------------------------------\n";
        std::cout << " GAME OVER. Care to try again?\n";
        std::cout << "---------------------------------------\n";

        if (CheckWin(Player::RED)) {
            std::cout << Color::RED  << Color::BOLD << "RED just crossed vertically!\n"    << Color::RESET;
        } else {
            std::cout << Color::BLUE << Color::BOLD << "BLUE just crossed horizontally!\n" << Color::RESET;
        }
    }
};

//----------------------------------------------------------

// MAIN FUNCTION
int main() {
    // Seed the random number generator for Monte Carlo simulations
    std::mt19937 rng(std::random_device{}());

    // Initialize the standard 11x11 board and start the game
    // (Adjust size number for a smaller/bigger board e.g. 7 or 5 for faster testing)
    HexBoard game(11);
    game.PlayGame(rng);

    return 0;
}