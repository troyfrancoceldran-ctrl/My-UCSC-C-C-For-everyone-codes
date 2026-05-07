/******************************************************************************
 * Project: Monte Carlo Simulation for 7-Card Poker Hand Probabilities
 * Author: Troy Celdran
 *
 * Description: 
 * This program defines a playing card using a struct containing an 
 * enumerated type for the suit and a short integer for the pip value. 
 * It features a shuffling function to randomize a standard 52-card deck. 
 * The primary purpose of the program is to use a Monte Carlo method 
 * (simulating at least 1,000,000 hands) to approximate the probabilities 
 * of drawing specific 7-card hands, including:
 * - No pair
 * - One pair
 * - Two pair
 * - Three of a kind
 * - Full house
 * - Four of a kind
 * - ETC... *See Probability Table in submission bin for more card combinations.
 ******************************************************************************/

#include <stdio.h>  // Required for printf
#include <stdlib.h> // Required for rand() and srand()
#include <time.h>   // Required for time() to seed the random number generator

// Enum struct for Suit Types
// Assigns an integer value (0-3) to represent the four standard card suits
typedef enum {
    Hearts,
    Spades,
    Clubs,
    Diamonds,
} Suit;

// Data Structure for the playing card
// Groups the suit and the numerical pip value (1-13) together
typedef struct {
    Suit suit_type;
    short pip_value; 
} Card;

// Initializes the first organized deck of cards
// Iterates through all 4 suits and all 13 pip values to build a 52-card deck
void initializeDeck(Card deck[]) {
    int Card_index = 0;
    
    // Outer loop goes through each suit (0 to 3)
    for (int s = 0; s < 4; s++) {
        // Inner loop goes through each pip value (0 to 12)
        for (int p = 0; p < 13; p++) {
            deck[Card_index].suit_type = (Suit)s; // Cast integer to Suit enum
            deck[Card_index].pip_value = p + 1;   // Values are 1 to 13, not 0 to 12
            Card_index++; // Move to the next slot in the deck array
        }
    }
}

// Function to shuffle deck using Fisher-Yates (Gold Standard Shuffling method)
// Iterates backwards through the deck, swapping each card with a random card before it
void shuffleDeck(Card deck[]) {
    Card temporary_card;
    for (int i = 51; i > 0; i--) {
        int j = rand() % (i + 1); // Pick a random index from 0 to i
        
        // Swap the card at index i with the randomly chosen card at index j
        temporary_card = deck[i];
        deck[i] = deck[j];
        deck[j] = temporary_card;
    }
}

// Function to evaluate the hand type based on the first 7 cards of the deck
int evaluateHand(Card deck[]) {
    // Histogram array to track frequencies of pip values.
    // Size is 14 so we can use index 1 for Aces up to index 13 for Kings.
    int pip_counts[14] = {0}; 
    
    // 1. Build the histogram
    // Look at the first 7 cards and tally how many times each pip value appears
    for (int p = 0; p < 7; p++) {
        pip_counts[deck[p].pip_value]++;
    }
    
    int pairs = 0, threes = 0, fours = 0;
    
    // 2. Scan the histogram
    // Check our tally array to see how many matching groups we found
    for (int i = 1; i <= 13; i++) { 
        if (pip_counts[i] == 2) { 
            pairs++; // Found a pair
        }  
        if (pip_counts[i] == 3) {
            threes++; // Found a three-of-a-kind
        }
        if (pip_counts[i] == 4) {
            fours++; // Found a four-of-a-kind
        }
    }

    // 3. Determine the hand
    // We check from the highest value hand down to the lowest to prevent double-counting.
    // Returns an integer code to main() so we can tally the result.
    if (fours == 1) {
        return 6; // Four of a Kind
    } 
    // A full house in 7 cards can be a 3-of-a-kind + pair, OR two 3-of-a-kinds
    else if ((threes >= 1 && pairs >= 1) || threes == 2) {
        return 5; // Full House
    } 
    else if (threes == 1) {
        return 4; // Three of a Kind
    } 
    else if (pairs >= 2) {
        return 3; // Two Pair
    } 
    else if (pairs == 1) {
        return 2; // One Pair
    } 
    else {
        return 1; // No Pair / High Card
    }
}

// Main Program Function
int main(void) {
    // Seed the random number generator using the current time
    // This ensures the deck shuffles differently on every program run
    srand(time(NULL));
    
    // Declare the array of 52 Card structures
    Card Deck[52];

    // Utilizes the initializeDeck Function to fill the array with valid cards
    initializeDeck(Deck);

    // Array to tally results. Size 7 allows us to directly use the return 
    // codes (1-6) from evaluateHand() as our exact array indexes.
    long tallies[7] = {0};

    // The Monte Carlo Loop: Run the simulation 1,000,000 times
    for (int c = 0; c < 1000000; c++) {
        shuffleDeck(Deck); // Randomize the deck
        
        // Evaluate the top 7 cards and get our hand code (1 to 6)
        int hand_result = evaluateHand(Deck); 
        
        // Increment the specific slot in our tally array matching the hand code
        tallies[hand_result]++;
    }

    // Store total hands as a double so division produces floating-point decimals
    double total_hands = 1000000.0; 

    // Output the final results
    printf("--- 7-Card Poker Monte Carlo Simulation ---\n");
    printf("Simulations run: 1,000,000\n\n");

    // Calculate and print percentages using the tallies array
    // (Count / Total) * 100 formats it as a percentage. %.4f prints 4 decimal places.
    printf("No Pair:         %.4f%%\n", (tallies[1] / total_hands) * 100);
    printf("One Pair:        %.4f%%\n", (tallies[2] / total_hands) * 100);
    printf("Two Pair:        %.4f%%\n", (tallies[3] / total_hands) * 100);
    printf("Three of a Kind: %.4f%%\n", (tallies[4] / total_hands) * 100);
    printf("Full House:      %.4f%%\n", (tallies[5] / total_hands) * 100);
    printf("Four of a Kind:  %.4f%%\n", (tallies[6] / total_hands) * 100);

    return 0;
}