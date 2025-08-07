#include "genetic_operations.h"
#include <stdlib.h>
#include <time.h>

// Two-point Crossover
// Performs crossover on the genome treating each gene as an indivisible unit.
// Genes outside the crossover window are copied from their respective parents
// while genes inside the window are swapped. If offspring2 is NULL the caller
// only expects one child.
void two_point_crossover(Gene* parent1, Gene* parent2, Gene* offspring1,
                         Gene* offspring2, int genome_length) {
    if (!parent1 || !parent2 || !offspring1 || genome_length <= 0) {
        return;
    }

    // Choose two crossover points along the genome
    int crossover1 = rand() % genome_length;
    int crossover2 = rand() % genome_length;
    if (crossover1 > crossover2) {
        int tmp = crossover1;
        crossover1 = crossover2;
        crossover2 = tmp;
    }

    for (int i = 0; i < genome_length; ++i) {
        if (i < crossover1 || i >= crossover2) {
            // Outside crossover region: copy from original parent
            offspring1[i] = parent1[i];
            if (offspring2) {
                offspring2[i] = parent2[i];
            }
        } else {
            // Inside crossover region: swap parental contribution
            offspring1[i] = parent2[i];
            if (offspring2) {
                offspring2[i] = parent1[i];
            }
        }
    }
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


