
#include <iostream>
#include <vector>
#include <random>
#include <time.h>

#include <unordered_set>
#include <bitset>
#include <list>

using namespace std;

// bitset requires compile-time alllcation with provided size or value;
const unsigned BITSET_MAX = 65536;

void get_binary_state(vector<vector<bool>>& v, bitset<BITSET_MAX>& state);

void toggle_binary(bitset<BITSET_MAX>& state, uint32_t y_pos, uint32_t x_pos, uint32_t y_length, uint32_t x_length);

/*
You are given a locked container represented as a two-dimensional grid of boolean values (true = locked, false = unlocked). 
Your task is to write an algorithm that fully unlocks the box, i.e., 
transforms the entire matrix into all false.

Implement the function:
bool openBox(uint32_t y, uint32_t x);
This function should:
    - Use the SecureBox public API (toggle, isLocked, getState).
    - Strategically toggle cells to reach a state where all elements are false.
    - Return true if the box remains locked, false if successfully unlocked.
You are not allowed to path or modify the SecureBox class.

Evaluation Criteria:
    - Functional correctness
    - Computational efficiency
    - Code quality, structure, and comments
    - Algorithmic insight and clarity
*/

class SecureBox
{
private:
    std::vector<std::vector<bool>> box;

public:

    //================================================================================
    // Constructor: SecureBox
    // Description: Initializes the secure box with a given size and 
    //              shuffles its state using a pseudo-random number generator 
    //              seeded with current time.
    //================================================================================
    SecureBox(uint32_t y, uint32_t x): ySize(y), xSize(x)
    {
        rng.seed(time(0));
        box.resize(y);
        for (auto& it : box)
            it.resize(x);
        shuffle();
    }

    //================================================================================
    // Method: toggle
    // Description: Toggles the state at position (x, y) and also all cells in the
    //              same row above and the same column to the left of it.
    //================================================================================
    void toggle(uint32_t y, uint32_t x)
    {
        box[y][x] = !box[y][x];
        for (uint32_t i = 0; i < xSize; i++)
            box[y][i] = !box[y][i];
        for (uint32_t i = 0; i < ySize; i++)
            box[i][x] = !box[i][x];
    }

    //================================================================================
    // Method: isLocked
    // Description: Returns true if any cell 
    //              in the box is true (locked); false otherwise.
    //================================================================================
    bool isLocked()
    {
        for (uint32_t x = 0; x < xSize; x++)
            for (uint32_t y = 0; y < ySize; y++)
                if (box[y][x])
                    return true;

        return false;
    }

    //================================================================================
    // Method: getState
    // Description: Returns a copy of the current state of the box.
    //================================================================================
    std::vector<std::vector<bool>> getState()
    {
        return box;
    }

private:
    std::mt19937_64 rng;
    uint32_t ySize, xSize;

    //================================================================================
    // Method: shuffle
    // Description: Randomly toggles cells in the box to 
    // create an initial locked state.
    //================================================================================
    void shuffle()
    {
        for (uint32_t t = rng() % 1000; t > 0; t--)
            toggle(rng() % ySize, rng() % xSize);
    }
};

struct Cell
{
    uint32_t y;
    uint32_t x;
    Cell(uint32_t _y, uint32_t _x): y(_y), x(_x){}
};

//================================================================================
// Function: openBox
// Description: Your task is to implement this function to unlock the SecureBox.
//              Use only the public methods of SecureBox (toggle, getState, isLocked).
//              You must determine the correct sequence of toggle operations to make
//              all values in the box 'false'. The function should return false if
//              the box is successfully unlocked, or true if any cell remains locked.
//================================================================================
bool openBox(uint32_t y, uint32_t x)
{
    SecureBox box(y, x);
    vector<vector<bool>> state = box.getState();
    uint32_t size = y * x;
    vector<vector<int>> T(size, vector<int>(size, 0));
    vector<int> A(size, 0);
    
    /**
     * T is an adjacency matrix to represent relationships between each node (cell), 
     * i.e. effect of toggle on a node. So the size of the matrix is number of elements * number_of_elements
     * T is a matrix from equation T*B = A, whereas A is original state matrix provided by the SecureBox::getState()
     *
     * Iterating over whole matrix 
     */
    for (uint32_t i = 0; i < y; i++) {
        for (uint32_t j = 0; j < x; j++) {

            // Index of the row representing relationship with all other cells 
            uint32_t idx = i * x + j;
            
            // Set 1s for elements in the same column with the cell
            for (uint32_t k = 0; k < x; k++)
                T[idx][i * x + k] = 1;
            
            // Set 1s for elements in the same row with the cell
            for (uint32_t k = 0; k < y; k++)
                T[idx][k * x + j] = 1;
            
            // Diagonal of the matrix is all 1s as the relationship
            // between node with itself is always true
            T[idx][idx] = 1;
        }
    }
    
    // Flatten initial state
    for (uint32_t i = 0; i < y; i++) {
        for (uint32_t j = 0; j < x; j++) {
            A[i * x + j] = state[i][j];
        }
    }
    

    /**
     * Gaussian elimination is a method of solving linear systems by building 
     * an upper triangular matrix which is then used to solve the system. 
     * The resulting vector A is then used to determine the operations needed
     * to solve the system. 
     * 
     */
    // Solve T * b = A using Gaussian elimination 
    for (uint32_t col = 0, row = 0; col < size && row < size; col++) {
        uint32_t pivot = row;

        // Finds the pivot, i.e. the row with leading 1 which is also called a coefficient
        while (pivot < size && !T[pivot][col])
            pivot++;

        // No 1 was found in the column
        if (pivot == size) continue;
        

        // Swapping the rows for moving the row with leading coefficient 1 above
        swap(T[row], T[pivot]);
        swap(A[row], A[pivot]);
        

        // Flipping the rows to turn the leading ones into zeroes
        for (uint32_t i = 0; i < size; i++) {

            // Check the i is not the same row and there are ones in the column in the others rows
            if (i != row && T[i][col]) {

                // XOR operation of found row on all the others
                for (uint32_t j = 0; j < size; j++)
                    T[i][j] ^= T[row][j];

                // XOR the augmented matrix
                A[i] ^= A[row];
            }
        }
        row++;
    }
    
    // Apply toggles
    for (uint32_t i = 0; i < size; i++) {
        if (A[i])
            box.toggle(i / x, i % x);
    }
    
    return box.isLocked();
}

int main(int argc, char* argv[])
{
    uint32_t y = std::atol(argv[1]);
    uint32_t x = std::atol(argv[2]);
    bool state = openBox(y, x);

    if (state)
        std::cout << "BOX: LOCKED!" << std::endl;
    else
        std::cout << "BOX: OPENED!" << std::endl;

    return state; 
}

/**
 * Converts the vector<vector<bool>> returned by SecureBox::getState method to bitset
 */
void get_binary_state(vector<vector<bool>>& v, bitset<BITSET_MAX>& state)
{

    uint32_t y = v.size();
    uint32_t x = v[0].size();

    for(unsigned i =0; i < y; i++)
    {
        for(unsigned j = 0; j < x; j++)
        {
            state.set(i * x + j, v[i][j]);
        };
    };
};

/** 
 * Binary version of SecureBox::toggle(uint32_t, uint32_t) method, 
 * to inverse the values of column x_pos and row y_pos of the matrix of size y_length*x_length
 */
void toggle_binary(bitset<BITSET_MAX>& state, uint32_t y_pos, uint32_t x_pos, uint32_t y_length, uint32_t x_length)
{
    for (uint32_t i = 0; i < y_length; i++)
    {
        state.flip(i * x_length + x_pos);  

    }
    for (uint32_t j = 0; j < x_length; j++)
    {
        state.flip(y_pos * x_length + j);
    }
    state.flip(y_pos * x_length + x_pos); 
}
