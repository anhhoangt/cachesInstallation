// Linear Congruential Generator (LCG) parameters
#define A 1664525
#define C 1013904223
#define M 4294967296 // 2^32

// Global variable to hold the seed
unsigned long seed;

// Function to generate a random number in a given range [min, max]
int genRand(int min, int max) 
{
    // Using the LCG algorithm
    seed = (A * seed + C) % M;
    return min + (seed % (max - min + 1));
}