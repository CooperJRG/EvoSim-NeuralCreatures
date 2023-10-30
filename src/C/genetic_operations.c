#include "genetic_operations.h"
#include <stdlib.h>
#include <time.h>


// Two-point Crossover
void two_point_crossover(Gene* parent1, Gene* parent2, Gene* offspring1, Gene* offspring2, int genome_length) {
    // Will treat each gene as a bit string for simplicity
    // Generate two random crossover points
     int i = rand() % genome_length;
        int crossover1 = rand() % 64;
        int crossover2 = rand() % 64;
        if (crossover1 > crossover2) {
            int temp = crossover1;
            crossover1 = crossover2;
            crossover2 = temp;
        }
        // Take all of the genes before the first crossover point from parent1
        uint64_t mask1 = ((1ULL << (crossover1)) - 1) << (64 - crossover1);
        // Take all of the genes between the two crossover points from parent2
        uint64_t mask2 = ((1ULL << (crossover2 - crossover1)) - 1) << (64 - crossover2);
        // Take all of the genes after the second crossover point from parent1
        uint64_t mask3 = ~(mask1 | mask2);
        offspring1[i].gene = (parent1[i].gene & mask1) | (parent2[i].gene & mask2) | (parent1[i].gene & mask3);
    
}

// Mutation
void mutate(Gene* genome, int genome_length) {
    // 1% chance to mutate the entire genome
    if (rand() / (float)RAND_MAX < MUTATION_RATE) {
        // Select a random gene from the genome
        int gene_to_mutate = rand() % genome_length;
        
        // Flip a random bit within that gene
        uint64_t mask = 1ULL << (rand() % 64);  // 64 bits in your gene
        genome[gene_to_mutate].gene ^= mask;
    }
}


