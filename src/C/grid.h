#ifndef GRID_H
#define GRID_H
#include <stdbool.h>
#include <stdint.h>

// Type definition for a single cell in the grid.
typedef struct {
    uint8_t occupied:1;
    uint8_t food:1;
    uint8_t poison:1;
    uint8_t wall:1;
    uint8_t sunlit:1;
    uint8_t water:1;
    uint8_t unused:2;  // Padding to make sure the total bitfield size is a multiple of 8
} CellFlags;


typedef struct {
    CellFlags flags;
    uint32_t creature_id;
} Cell;

// Type definition for the entire grid.
typedef struct {
    Cell* cells;  // 2D array of cells
    uint16_t width;  // Width of the grid
    uint16_t height;  // Height of the grid
    uint32_t num_creatures; // Number of creatures in the grid
    uint64_t num_generations; // Number of generations that have passed
    uint32_t max_steps; // Maximum number of steps to run
    uint32_t max_creatures; // Maximum number of creatures to allow
    uint32_t num_genomes; // Number of genomes to start with
    uint32_t num_creatures_alive_last_gen; // Number of creatures alive in the last generation
} Grid;

/**
 * Initialize a new grid with the given dimensions.
 * 
 * @param width Width of the grid.
 * @param height Height of the grid.
 * @return Pointer to the newly created Grid.
 */
Grid* initialize_grid(uint16_t width, uint16_t height, uint32_t max_creatures, uint32_t max_steps, uint32_t num_genomes);

/**
 * Deallocate memory associated with the grid.
 * 
 * @param grid Pointer to the grid to be deallocated.
 */
void free_grid(Grid* grid);


/**
 * Retrieve the cell at the given coordinates.
 * 
 * @param grid Pointer to the grid.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @return Pointer to the Cell at the given coordinates.
 */
Cell* get_cell(Grid* grid, uint16_t x, uint16_t y);

/**
 * Output the grid state to a CSV file for visualization.
 * 
 * @param grid Pointer to the grid.
 * @param filename Name of the output CSV file.
 * @return 0 on success, non-zero on failure.
 */
int output_grid_to_csv(Grid* grid, const char* filename);

// ... Add any other utility functions or operations that should be supported by the grid.

/**
 * Scatter a number of food items randomly across the grid.
 *
 * @param grid Pointer to the grid.
 * @param amount Number of food cells to place.
 */
void scatter_food(Grid* grid, uint32_t amount);

#endif // GRID_H
