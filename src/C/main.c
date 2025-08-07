#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grid.h"
#include "neuron_encoding.h"
#include "genetic_operations.h"
#include "simulation.h"

int main() {
    // Initialize random seed
    srand(time(NULL));

    // Grid parameters
    uint16_t width = 300;
    uint16_t height = 300;
    uint32_t max_creatures = 200;
    uint32_t max_steps = 300 * 10000;
    uint32_t num_genomes = 32;

    printf("Initializing grid...\n");
    // Initialize the grid
    Grid* grid = initialize_grid(width, height, max_creatures, 300, num_genomes);
    if (!grid) {
        fprintf(stderr, "Grid initialization failed.\n");
        return 1;
    }

    printf("Initializing creatures...\n");
    // Initialize creatures
    Creature* creatures = malloc(max_creatures * sizeof(Creature));
    if (!creatures) {
        fprintf(stderr, "Creature array initialization failed.\n");
        free_grid(grid);
        return 1;
    }

    printf("Initializing genomes...\n");
    // Spawn creatures on the grid
    spawn_creatures(grid, creatures);
    printf("Gen %d is begining.\n", 0);

    int gen = 0;
    // Run the simulation for max_steps
    for (uint32_t step = 0; step < max_steps; ++step) {
        update_grid(grid, creatures);
        if ( step != 0 && (step)% 300 == 0) {
            printf("Gen %d:\n", gen);
            if (grid->num_creatures_alive_last_gen > 0) {
                printf("Survival Rate: %0.2f%%\n", ((float)grid->num_creatures_alive_last_gen / max_creatures) * 100);
                // Pick a random creature, show its genome
                int creature_index = rand() % grid->num_creatures_alive_last_gen;
                if (creatures[creature_index].brain) {
                    FILE *neuron_file = fopen("neurons.csv", "w");
                    fprintf(neuron_file, "Index,Type,ID,Label\n");

                    for (int i = 0; i < creatures[creature_index].brain->total_neurons; ++i) {
                        Neuron* neuron = &creatures[creature_index].brain->neurons[i];
                        fprintf(neuron_file, "%d,%s,%u,%s\n", i, neuron_type_to_string(neuron->type), neuron->id, neuron_id_to_string(neuron->id));
                    }

                    fclose(neuron_file);

                    FILE *connection_file = fopen("connections.csv", "w");
                    fprintf(connection_file, "SourceID,TargetID,Weight,ActivationFunction\n");

                    for (int i = 0; i < creatures[creature_index].brain->total_neurons; ++i) {
                        Neuron* neuron = &creatures[creature_index].brain->neurons[i];
                        for (int j = 0; j < neuron->num_connections; ++j) {
                            fprintf(connection_file, "%u,%u,%f,%s\n", neuron->id, neuron->connections[j].id, neuron->connections[j].weight, activation_function_to_string(neuron->connections[j].activation_function));
                        }
                    }

                    fclose(connection_file);
                }
            } else {
                printf("Survival Rate: 0.00%%\n");
            }
            gen++;
        }
    }

    // Clean up
    for (int i = 0; i < max_creatures; ++i) {
        free(creatures[i].genome);
        // If you dynamically allocate the neural network or other fields, free them here
    }
    free(creatures);
    free_grid(grid);

    return 0;
}
