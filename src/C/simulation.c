#include "grid.h"
#include "neuron_encoding.h"
#include "genetic_operations.h"
#include "simulation.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>



/**
 * Spawns a given number of creatures on the provided grid.
 *
 * @param grid The grid on which to spawn the creatures.
 * @param creatures An array of Creature objects to spawn.
 * @param num_creatures The number of creatures to spawn.
 */
void spawn_creatures(Grid* grid, Creature* creatures){
    int num_creatures = grid->max_creatures;
    int genome_length = grid->num_genomes;
    // Cap the number of creatures at the maximum number of cells in the grid
    if (num_creatures > grid->width * grid->height) {
        num_creatures = grid->width * grid->height;
    }
    // Initialize the grid
    grid->num_creatures = num_creatures;
    for (int i = 0; i < num_creatures; ++i) {
        // Place creature in a random location
        int x = rand() % grid->width;
        int y = rand() % grid->height;
        while (get_cell(grid, x, y)->flags.occupied) {
            x = rand() % grid->width;
            y = rand() % grid->height;
        }
        get_cell(grid, x, y)->flags.occupied = 1;
        get_cell(grid, x, y)->creature_id = i + 1;
        spawn_creature(&creatures[i], genome_length);
        creatures[i].position.x = x;
        creatures[i].position.y = y;
        creatures[i].energy = 100;
        creatures[i].id = i + 1;
    }
}

void spawn_creature(Creature* creature, int genome_length) {
    creature->genome_length = genome_length;
    creature->genome = malloc(genome_length * sizeof(Gene));
    if (!creature->genome) {
        return;  // Allocation failed
    }
    /*
     * Generates a random genome for a creature by setting each gene to a 64-bit integer
     * created by concatenating two 32-bit random integers.
     */
    for (int i = 0; i < genome_length; ++i) {
        uint32_t random1 = rand();  // Generate a random 32-bit integer
        uint32_t random2 = rand();  // Generate another random 32-bit integer
        creature->genome[i].gene = ((uint64_t)random1 << 32) | random2;
    }
    creature->brain = initialize_neural_network(creature->genome, genome_length);
    if (!creature->brain) {
        creature->energy = 0;
        return;
    }
    creature->position.x = 0;
    creature->position.y = 0;
    creature->age = 0;
    creature->generation = 0;
}

/**
 * @brief Updates the given grid by simulating one time step of the creatures' behavior.
 * 
 * @param grid A pointer to the grid to be updated.
 */
void update_grid(Grid* grid, Creature* creatures){
    // If the maximum number of steps has been reached, reset the grid
    if (grid->num_generations >= grid->max_steps) {
        grid->num_generations = 0;
        mate_creatures(grid, creatures);
    } else {
        grid->num_generations++;
        // Iterate through each cell in the grid
        for (int i = 0; i < grid->width * grid->height; ++i) {
            // Check if the cell contains a creature
            if (grid->cells[i].flags.occupied) {
                // Check if Creature ID is valid
                if (grid->cells[i].creature_id <= 0) {
                    grid->cells[i].flags.occupied = 0;
                    continue;
                }
                // Fetch the creature in the cell
                Creature* creature = &creatures[grid->cells[i].creature_id - 1];
                // Update the creature's state
                update_creature(grid, *creature);
            }
        }
    }
    
}

/**
 * @brief Mates the creatures in the given grid.
 * 
 * @param grid A pointer to the grid containing the creatures to mate.
 */
void mate_creatures(Grid* grid, Creature* creatures) {
    // Count the number of creatures that are alive and in the top half of the grid
    int num_creatures = 0;
    for (int i = 0; i < grid->max_creatures; ++i) {
        if (creatures[i].position.y < grid->height / 2 && creatures[i].energy > 0) {
            num_creatures++;
        }
    }
    grid->num_creatures_alive_last_gen = num_creatures;
    int genome_length = creatures[0].genome_length;  // Assuming all creatures have the same genome length

    Creature* new_creatures = malloc(grid->max_creatures * sizeof(Creature));
    if (!new_creatures) {
        // Handle allocation failure
        return;
    }

    int new_creature_count = 0;
    while (new_creature_count < grid->max_creatures) {
        Creature* parent1 = NULL;
        Creature* parent2 = NULL;

        // Find parent1
        while (!parent1) {
            int rand_id = rand() % grid->max_creatures;  // Generate random creature id
            if (creatures[rand_id].position.y < grid->height / 2 && creatures[rand_id].energy > 0) {
                parent1 = &creatures[rand_id];
            }
        }

        // Find parent2
        while (!parent2) {
            int rand_id = rand() % grid->max_creatures;  // Generate random creature id
            if (creatures[rand_id].position.y < grid->height / 2 && creatures[rand_id].energy > 0) {
                parent2 = &creatures[rand_id];
            }
        }

        // Perform mating to produce one offspring
        Gene* offspring_genome = malloc(genome_length * sizeof(Gene));
        if (!offspring_genome) {
            // Handle allocation failure
            return;
        }

        // Two-point crossover to produce one offspring
        two_point_crossover(parent1->genome, parent2->genome, offspring_genome, NULL, genome_length);

        // Mutation
        mutate(offspring_genome, genome_length);

        NeuralNetwork* brain = initialize_neural_network(offspring_genome, genome_length);
        if (!brain) {
            free(offspring_genome);
            continue;
        }

        // Assign the offspring genome to a new creature
        new_creatures[new_creature_count].genome = offspring_genome;
        new_creatures[new_creature_count].genome_length = genome_length;
        new_creatures[new_creature_count].brain = brain;
        new_creatures[new_creature_count].energy = 100;
        new_creatures[new_creature_count].age = 0;

        new_creature_count++;
    }

    // Overwrite old creatures with new creatures
    for (int i = 0; i < grid->max_creatures; ++i) {
        // Free the old genome
        free(creatures[i].genome);

        // Copy the new genome
        creatures[i].genome = new_creatures[i].genome;
        creatures[i].genome_length = new_creatures[i].genome_length;
        creatures[i].brain = new_creatures[i].brain;
        creatures[i].energy = new_creatures[i].energy;
        creatures[i].age = new_creatures[i].age;
        creatures[i].generation++;
}

    // Free the new_creatures array, but not the genomes
    free(new_creatures);


    // Place each new creature in a random free cell
    for (int i = 0; i < grid->max_creatures; ++i) {
        int x, y;
        do {
            x = rand() % grid->width;
            y = rand() % grid->height;
        } while (get_cell(grid, x, y)->flags.occupied);

        creatures[i].position.x = x;
        creatures[i].position.y = y;
        get_cell(grid, x, y)->flags.occupied = 1;
        get_cell(grid, x, y)->creature_id = i + 1;
    }
    grid->num_creatures = grid->max_creatures;
}

/**
 * @brief Updates the state of a creature in the given grid.
 * 
 * @param grid Pointer to the grid containing the creature.
 * @param creature The creature to update.
 */
void update_creature(Grid* grid, Creature creature){
    // Fetch the cell the creature is currently in
    Cell* cell = get_cell(grid, creature.position.x, creature.position.y);
    if (!creature.brain) {
        cell->flags.occupied = 0;
        cell->creature_id = 0;
        grid->num_creatures--;
        return;
    }
    // Check if the creature is dead
    if (creature.energy <= 0) {
        cell->flags.occupied = 0;
        cell->creature_id = 0;
        grid->num_creatures--;
        return;
    }
    // Update the creature's age
    creature.age++;
    // Update the creature's energy
    creature.energy -= 0.01;
    // Fetch the creature's brain
    NeuralNetwork* brain = creature.brain;
    // Fetch the creature's sensory neurons
    uint16_t* sensory_ids = brain->sensory_ids;
    // Iterate through each sensory neuron
    for (int i = 0; i < brain->num_sensory_neurons; ++i) {
        // Fetch the data for the sensory neuron
        float data = get_sensory_data(sensory_ids[i], creature.position.x, creature.position.y, grid);
        // Fetch the sensory neuron
        Neuron* neuron = find_neuron_by_id(brain->neurons, brain->total_neurons, sensory_ids[i]);
        // Update the sensory neuron's data
        neuron->data = data;
    }
    // Update the creature's brain
    propagate_signal(brain);
    // Fetch the creature's output neurons
    uint16_t* output_ids = brain->output_ids;
    // Action ID to perform
    uint16_t action_id = 0;
    float data = -FLT_MAX;
    // Iterate through each output neuron
    for (int i = 0; i < brain->num_output_neurons; ++i) {
        // Fetch the output neuron
        Neuron* neuron = find_neuron_by_id(brain->neurons, brain->total_neurons, output_ids[i]);
        // Keep track of the highest data value
        if (data < neuron->data) {
            action_id = output_ids[i];
            data = neuron->data;
        }
    }
    // If the data is above the activation threshold, perform the action
    if (action_id >15 && data > find_neuron_by_id(brain->neurons, brain->total_neurons, action_id)->activation_threshold) {
        perform_action(action_id, grid, &creature);
    }
}

void perform_action(uint16_t action_id, Grid* grid, Creature* creature) {
    Cell* cell;
    if (action_id == M_r){
        // Set the action ID to a random movement action (21 to 28)
        action_id = rand() % 8 + 21;
    }
    switch (action_id) {
        case M_n:
            if (creature->position.y > 0) {
                cell = get_cell(grid, creature->position.x, creature->position.y - 1);
                if (!cell->flags.occupied) {
                    cell->flags.occupied = 1;
                    cell->creature_id = creature->id;
                    get_cell(grid, creature->position.x, creature->position.y)->flags.occupied = 0;
                    get_cell(grid, creature->position.x, creature->position.y)->creature_id = 0;
                    creature->position.y--;
                }
            }
            break;
        case M_ne:
            if (creature->position.x < grid->width - 1 && creature->position.y > 0) {
                cell = get_cell(grid, creature->position.x + 1, creature->position.y - 1);
                if (!cell->flags.occupied) {
                    cell->flags.occupied = 1;
                    cell->creature_id = creature->id;
                    get_cell(grid, creature->position.x, creature->position.y)->flags.occupied = 0;
                    get_cell(grid, creature->position.x, creature->position.y)->creature_id = 0;
                    creature->position.x++;
                    creature->position.y--;
                }
            }
            break;
        case M_e:
            if (creature->position.x < grid->width - 1) {
                cell = get_cell(grid, creature->position.x + 1, creature->position.y);
                if (!cell->flags.occupied) {
                    cell->flags.occupied = 1;
                    cell->creature_id = creature->id;
                    get_cell(grid, creature->position.x, creature->position.y)->flags.occupied = 0;
                    get_cell(grid, creature->position.x, creature->position.y)->creature_id = 0;
                    creature->position.x++;
                }
            }
            break;
        case M_se:
            if (creature->position.x < grid->width - 1 && creature->position.y < grid->height - 1) {
                cell = get_cell(grid, creature->position.x + 1, creature->position.y + 1);
                if (!cell->flags.occupied) {
                    cell->flags.occupied = 1;
                    cell->creature_id = creature->id;
                    get_cell(grid, creature->position.x, creature->position.y)->flags.occupied = 0;
                    get_cell(grid, creature->position.x, creature->position.y)->creature_id = 0;
                    creature->position.x++;
                    creature->position.y++;
                }
            }
            break;
        case M_s:
            if (creature->position.y < grid->height - 1) {
                cell = get_cell(grid, creature->position.x, creature->position.y + 1);
                if (!cell->flags.occupied) {
                    cell->flags.occupied = 1;
                    cell->creature_id = creature->id;
                    get_cell(grid, creature->position.x, creature->position.y)->flags.occupied = 0;
                    get_cell(grid, creature->position.x, creature->position.y)->creature_id = 0;
                    creature->position.y++;
                }
            }
            break;
        case M_sw:
            if (creature->position.x > 0 && creature->position.y < grid->height - 1) {
                cell = get_cell(grid, creature->position.x - 1, creature->position.y + 1);
                if (!cell->flags.occupied) {
                    cell->flags.occupied = 1;
                    cell->creature_id = creature->id;
                    get_cell(grid, creature->position.x, creature->position.y)->flags.occupied = 0;
                    get_cell(grid, creature->position.x, creature->position.y)->creature_id = 0;
                    creature->position.x--;
                    creature->position.y++;
                }
            }
            break;
        case M_w:
            if (creature->position.x > 0) {
                cell = get_cell(grid, creature->position.x - 1, creature->position.y);
                if (!cell->flags.occupied) {
                    cell->flags.occupied = 1;
                    cell->creature_id = creature->id;
                    get_cell(grid, creature->position.x, creature->position.y)->flags.occupied = 0;
                    get_cell(grid, creature->position.x, creature->position.y)->creature_id = 0;
                    creature->position.x--;
                }
            }
            break;
        case M_nw:
            if (creature->position.x > 0 && creature->position.y > 0) {
                cell = get_cell(grid, creature->position.x - 1, creature->position.y - 1);
                if (!cell->flags.occupied) {
                    cell->flags.occupied = 1;
                    cell->creature_id = creature->id;
                    get_cell(grid, creature->position.x, creature->position.y)->flags.occupied = 0;
                    get_cell(grid, creature->position.x, creature->position.y)->creature_id = 0;
                    creature->position.x--;
                    creature->position.y--;
                }
            }
            break;
    }
}

float get_sensory_data(NeuronID id, uint16_t x, uint16_t y, Grid* grid) {
    Cell* cell;
    int dx, dy;  // Delta x and y for calculating positions
    float data = -FLT_MAX;  // Default value
    switch (id) {
        case L_n:
            dy = y - 1;
            if (dy >= 0) {
                cell = get_cell(grid, x, dy);
                data = cell->flags.occupied ? -1.0 : 0.0;
            } else {
                data = -2.0;  // Out of bounds
            }
            break;
        case L_ne:
            dx = x + 1;
            dy = y - 1;
            if (dx < grid->width && dy >= 0) {
                cell = get_cell(grid, dx, dy);
                data = cell->flags.occupied ? -1.0 : 0.0;
            } else {
                data = -2.0;  // Out of bounds
            }
            break;
        case L_e:
            dx = x + 1;
            if (dx < grid->width) {
                cell = get_cell(grid, dx, y);
                data = cell->flags.occupied ? -1.0 : 0.0;
            } else {
                data = -2.0;  // Out of bounds
            }
            break;
        case L_se:
            dx = x + 1;
            dy = y + 1;
            if (dx < grid->width && dy < grid->height) {
                cell = get_cell(grid, dx, dy);
                data = cell->flags.occupied ? -1.0 : 0.0;
            } else {
                data = -2.0;  // Out of bounds
            }
            break;
        case L_s:
            dy = y + 1;
            if (dy < grid->height) {
                cell = get_cell(grid, x, dy);
                data = cell->flags.occupied ? -1.0 : 0.0;
            } else {
                data = -2.0;  // Out of bounds
            }
            break;
        case L_sw:
            dx = x - 1;
            dy = y + 1;
            if (dx >= 0 && dy < grid->height) {
                cell = get_cell(grid, dx, dy);
                data = cell->flags.occupied ? -1.0 : 0.0;
            } else {
                data = -2.0;  // Out of bounds
            }
            break;
        case L_w:
            dx = x - 1;
            if (dx >= 0) {
                cell = get_cell(grid, dx, y);
                data = cell->flags.occupied ? -1.0 : 0.0;
            } else {
                data = -2.0;  // Out of bounds
            }
            break;
        case L_nw:
            dx = x - 1;
            dy = y - 1;
            if (dx >= 0 && dy >= 0) {
                cell = get_cell(grid, dx, dy);
                data = cell->flags.occupied ? -1.0 : 0.0;
            } else {
                data = -2.0;  // Out of bounds
            }
            break;
        case LW_n:
            for (dy = y - 1; dy >= 0; dy--) {
                cell = get_cell(grid, x, dy);
                if (cell->flags.wall) {
                    data = y - dy;  // Distance to the wall
                    break;
                }
            }
            if (dy < 0) {  // Reached the boundary without finding a wall
                data = y;
            }
            break;
        case LW_ne:
            for (dx = x + 1, dy = y - 1; dx < grid->width && dy >= 0; dx++, dy--) {
                cell = get_cell(grid, dx, dy);
                if (cell->flags.wall) {
                    data = y - dy;  // Distance to the wall
                    break;
                }
            }
            if (dx >= grid->width || dy < 0) {  // Reached the boundary without finding a wall
                data = y < grid->width - x ? y : grid->width - x;
            }
            break;
        case LW_e:
            for (dx = x + 1; dx < grid->width; dx++) {
                cell = get_cell(grid, dx, y);
                if (cell->flags.wall) {
                    data = dx - x;  // Distance to the wall
                    break;
                }
            }
            if (dx >= grid->width) {  // Reached the boundary without finding a wall
                data = grid->width - x;
            }
            break;
        case LW_se:
            for (dx = x + 1, dy = y + 1; dx < grid->width && dy < grid->height; dx++, dy++) {
                cell = get_cell(grid, dx, dy);
                if (cell->flags.wall) {
                    data = dy - y;  // Distance to the wall
                    break;
                }
            }
            if (dx >= grid->width || dy >= grid->height) {  // Reached the boundary without finding a wall
                data = y < grid->width - x ? grid->height - y : grid->width - x;
            }
            break;
        case LW_s:
            for (dy = y + 1; dy < grid->height; dy++) {
                cell = get_cell(grid, x, dy);
                if (cell->flags.wall) {
                    data = dy - y;  // Distance to the wall
                    break;
                }
            }
            if (dy >= grid->height) {  // Reached the boundary without finding a wall
                data = grid->height - y;
            }
            break;
        case LW_sw:
            for (dx = x - 1, dy = y + 1; dx >= 0 && dy < grid->height; dx--, dy++) {
                cell = get_cell(grid, dx, dy);
                if (cell->flags.wall) {
                    data = dy - y;  // Distance to the wall
                    break;
                }
            }
            if (dx < 0 || dy >= grid->height) {  // Reached the boundary without finding a wall
                data = y < x ? grid->height - y : x;
            }
            break;
        case LW_w:
            for (dx = x - 1; dx >= 0; dx--) {
                cell = get_cell(grid, dx, y);
                if (cell->flags.wall) {
                    data = x - dx;  // Distance to the wall
                    break;
                }
            }
            if (dx < 0) {  // Reached the boundary without finding a wall
                data = x;
            }
            break;
        case LW_nw:
            for (dx = x - 1, dy = y - 1; dx >= 0 && dy >= 0; dx--, dy--) {
                cell = get_cell(grid, dx, dy);
                if (cell->flags.wall) {
                    data = x - dx;  // Distance to the wall
                    break;
                }
            }
            if (dx < 0 || dy < 0) {  // Reached the boundary without finding a wall
                data = y < x ? y : x;
            }
            break;
        default:
            // Do nothing or throw an error
            break;
    }
    if (data > 0) {
        data = 1.0 / data;
    }
    return data;
}
