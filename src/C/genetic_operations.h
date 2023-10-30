#ifndef GENETIC_OPERATIONS_H
#define GENETIC_OPERATIONS_H

#include "gene_encoding.h"  // Assuming this is where your Gene struct is defined

// Mutation rate constant
#define MUTATION_RATE 0.0001  // A 0.1% mutation rate

// Function Prototypes

/**
 * Performs two-point crossover on two parent genomes to create two offspring genomes.
 */
void two_point_crossover(Gene* parent1, Gene* parent2, Gene* offspring1, Gene* offspring2, int genome_length);

/**
 * Mutates a given genome based on the mutation rate.
 */
void mutate(Gene* genome, int genome_length);

#endif // GENETIC_OPERATIONS_H
