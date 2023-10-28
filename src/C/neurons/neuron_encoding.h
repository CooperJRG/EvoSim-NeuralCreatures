#ifndef NEURON_ENCODING_H
#define NEURON_ENCODING_H

#include <stdint.h>
// Neuron Types Enum
typedef enum {
    // Look neurons for sensing objects in 8 directions
    L_n, L_ne, L_e, L_se, L_s, L_sw, L_w, L_nw,
    
    // Look for Wall neurons for detecting proximity to boundaries in 8 directions
    LW_n, LW_ne, LW_e, LW_se, LW_s, LW_sw, LW_w, LW_nw,
    
    // Internal neurons for processing sensory inputs and actions
    I_0, I_1, I_2, I_3, I_4,

    // Move neurons for moving in each of the 8 directions
    M_n, M_ne, M_e, M_se, M_s, M_sw, M_w, M_nw,
    
    // Move Random neuron for random movement
    M_r,
    
    // Count of total neurons, useful for array sizes and loops
    TOTAL_NEURONS
} NeuronID;

// Enum to define the neuron type
typedef enum {
    SENSORY,
    INTERNAL,
    CONSTANT,
    OUTPUT,
} NeuronType;


// General struct for neurons
typedef struct {
    NeuronType type;      // Type of the neuron (sensory, internal, output, etc.)
    NeuronID id;          // Unique identifier for the neuron
    int32_t data;      // Integer holding numeric data for the neuron
} Neuron;

#endif // NEURON_ENCODING_H
