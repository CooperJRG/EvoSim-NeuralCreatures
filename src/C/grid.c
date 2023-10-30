#include "grid.h"
#include <stdlib.h>
#include <stdio.h>


/**
 * Initializes a new grid with the given width and height.
 * 
 * @param width The width of the grid.
 * @param height The height of the grid.
 * @return A pointer to the newly initialized grid, or NULL if allocation failed.
 */
Grid* initialize_grid(uint16_t width, uint16_t height, uint32_t max_creatures, uint32_t max_steps, uint32_t num_genomes){
    Grid* grid = malloc(sizeof(Grid));
    if (!grid) {
        return NULL;  // Allocation failed
    }
    grid->width = width;
    grid->height = height;
    grid->num_creatures = 0;
    grid->max_creatures = max_creatures;
    grid->max_steps = max_steps;
    grid->num_generations = 0;
    grid->num_genomes = num_genomes;
    grid->cells = malloc(width * height * sizeof(Cell));
    if (!grid->cells) {
        free(grid);
        return NULL;  // Allocation failed
    }
    // Initialize cell data to defaults
    for (uint32_t i = 0; i < width * height; ++i) {
        grid->cells[i].flags.occupied = 0;
        grid->cells[i].flags.food = 0;
        grid->cells[i].flags.poison = 0;
        grid->cells[i].flags.wall = 0;
        grid->cells[i].flags.sunlit = 0;
        grid->cells[i].flags.water = 0;
        grid->cells[i].creature_id = 0;
    }
    return grid;
}

/**
 * Deallocate memory associated with the grid.
 * 
 * @param grid Pointer to the grid to be deallocated.
 */
void free_grid(Grid* grid){
    free(grid->cells);
    free(grid);
}


/**
 * Retrieve the cell at the given coordinates.
 * 
 * @param grid Pointer to the grid.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @return Pointer to the Cell at the given coordinates.
 */
Cell* get_cell(Grid* grid, uint16_t x, uint16_t y){
    return &grid->cells[y * grid->width + x];
}

/**
 * Output the grid state to a CSV file for visualization.
 * 
 * @param grid Pointer to the grid.
 * @param filename Name of the output CSV file.
 * @return 0 on success, non-zero on failure.
 */
int output_grid_to_csv(Grid* grid, const char* filename){
    FILE *file = fopen(filename, "w");
    if (!file) {
        return 1;  // File open failed
    }
    fprintf(file, "X,Y,Occupied,Food,Poison,Wall,Sunlit,Water,CreatureID\n");
    for (uint16_t y = 0; y < grid->height; ++y) {
        for (uint16_t x = 0; x < grid->width; ++x) {
            Cell* cell = get_cell(grid, x, y);
            fprintf(file, "%d,%d,%d,%d,%d,%d,%d,%d,%d\n", x, y, cell->flags.occupied, cell->flags.food, cell->flags.poison, cell->flags.wall, cell->flags.sunlit, cell->flags.water, cell->creature_id);
        }
    }
    fclose(file);
    return 0;
}