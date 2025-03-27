
#include <iostream>
#include <vector>
#include <random>
#include <time.h>

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

//================================================================================
// Function: openBox
// Description: Your task is to implement this function to unlock the SecureBox.
//              Use only the public methods of SecureBox (toggle, getState, isLocked).
//              You must determine the correct sequence of toggle operations to make
//              all values in the box 'false'. The function should return false if
//              the box is successfully unlocked, or true if any cell remains locked.
//================================================================================
// bool openBox(uint32_t y, uint32_t x)
// {
//     SecureBox box(y, x);

//     // PLEASE, PROVIDE YOUR SOLUTION HERE
//     // ...

//     return box.isLocked();
// }

using namespace std;

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <bitset>

const unsigned BITSET_MAX = 1000000;

void print(std::vector<std::vector<bool>> v)
{
    for(unsigned i = 0; i < v.size(); i++)
    {
        for(unsigned j = 0; j < v[0].size(); j++)
        {
            if(v[i][j])
            {
                std::cout << "1";
            }else{
                std::cout << "0";
            };
            std::cout << "|";
        };  
        std::cout << std::endl;
    };

};

void get_binary_state(vector<vector<bool>>& v, bitset<BITSET_MAX>& state)
{

    uint32_t y = v.size();
    uint32_t x = v[0].size();

    for(unsigned i =0; i < y; i++)
    {
        for(unsigned j = 0; j < x; j++)
        {
            state.set(j * y + i, v[i][j]);
        };
    };
};

void toggle_binary(bitset<BITSET_MAX>& state, uint32_t y_pos, uint32_t x_pos, uint32_t y_length, uint32_t x_length)
{
    for (uint32_t i = 0; i < y_length; i++)
    {
        state.flip(x_pos * y_length + i);  // Toggle column
    }
    for (uint32_t j = 0; j < x_length; j++)
    {
        state.flip(j * y_length + y_pos);  // Toggle row
    }
    state.flip(x_pos * y_length + y_pos);  // Toggle itself
}


struct Cell
{
    uint32_t y;
    uint32_t x;
};

bool openBox(uint32_t y, uint32_t x)
{
    SecureBox box(y, x);

    vector<vector<bool>> state = box.getState();

    bitset<BITSET_MAX> binary_state;
    get_binary_state(state, binary_state);

    queue<bitset<BITSET_MAX>> states;
    unordered_set<bitset<BITSET_MAX>> visited;
    

    states.push(binary_state);
    visited.insert(binary_state);

    uint32_t tries = 0;

    while(binary_state.none() ||  !states.empty())
    {
        tries++;
        binary_state = states.front();
        states.pop();

        if(binary_state.none())
        {
            break;
        } 

        for(uint32_t i = 0; i < y; i++)
        {
            for(uint32_t j = 0; j < x; j++)
            {
                if(binary_state.test(i * y + j))
                {
                     box.toggle(i, j);

                     state = box.getState();
                     get_binary_state(state, binary_state);

                    if(!visited.count(binary_state)){
                        visited.insert(binary_state);
                        states.push(binary_state);
                    }else{
                        box.toggle(i, j);
                        state = box.getState();
                        get_binary_state(state, binary_state);

                    }
                
                }
               
            };
        }
    };


    std::cout << tries << " tries\n";

    print(state);
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