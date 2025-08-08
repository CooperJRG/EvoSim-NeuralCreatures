#ifndef SIMULATION_H
#define SIMULATION_H
#include <stdint.h>
#include "neuron_encoding.h"
#include "grid.h"

typedef struct{
    uint16_t x;
    uint16_t y;
} Position;

typedef struct{
    Position position;
    NeuralNetwork* brain;
    float energy;
    uint32_t id;
    uint32_t age;
    uint32_t generation;
    int genome_length;
    Gene* genome;
} Creature;

/**
 * @brief Updates the given grid by simulating one time step of the creatures' behavior.
 * 
 * @param grid A pointer to the grid to be updated.
 */
void update_grid(Grid* grid, Creature* creatures);
/**
 * Spawns a given number of creatures on the provided grid.
 *
 * @param grid The grid on which to spawn the creatures.
 * @param creatures An array of Creature objects to spawn.
 * @param num_creatures The number of creatures to spawn.
 */
void spawn_creatures(Grid* grid, Creature* creatures);
/**
 * @brief Updates the state of a creature in the given grid.
 * 
 * @param grid Pointer to the grid containing the creature.
 * @param creature The creature to update.
 */
void update_creature(Grid* grid, Creature* creature);
void spawn_creature(Creature* creature, int genome_length);
/**
 * @brief Mates the creatures in the given grid.
 * 
 * @param grid A pointer to the grid containing the creatures to mate.
 */
void mate_creatures(Grid* grid, Creature* creatures);
float get_sensory_data(NeuronID id, uint16_t x, uint16_t y, Grid* grid);
void perform_action(uint16_t action_id, Grid* grid, Creature* creature);

#endif
