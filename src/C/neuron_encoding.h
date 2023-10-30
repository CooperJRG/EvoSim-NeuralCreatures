#ifndef NEURON_ENCODING_H
#define NEURON_ENCODING_H

#include <stdint.h>
#include <stdbool.h>
#include "gene_encoding.h"


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

// Enum to define the activation function type
typedef enum {
    RELU,
    SIGMOID,
    TANH,
    // Add more as needed
} ActivationFunctionType;

// Struct to represent a single connection from this neuron to another
typedef struct {
    NeuronID id;  // Pointer to the target neuron
    float weight;    // Weight of this connection
    ActivationFunctionType activation_function; // Type of activation function to use
} Connection;

// General struct for neurons
typedef struct {
    NeuronType type;                // Type of the neuron (sensory, internal, output, etc.)
    NeuronID id;                    // Unique identifier for the neuron
    float data;                     // Float holding numeric data for the neuron
    float activation_threshold;     // Threshold for activation (relevant mainly for output neurons)
    Connection* connections;        // Dynamic array of connections
    int num_connections;            // Number of connections for memory management
} Neuron;

typedef struct NeuralNetwork {
    Neuron* neurons;              // Array of neurons
    int total_neurons;            // Total number of neurons
    int num_sensory_neurons;      // Number of sensory neurons
    int num_output_neurons;       // Number of output neurons
    uint16_t* sensory_ids;        // IDs of sensory neurons (if you find it useful)
    uint16_t* output_ids;         // IDs of output neurons (if you find it useful)
} NeuralNetwork;

int is_unique_id(uint16_t* unique_neurons, int* unique_count, uint16_t id);
void initialize_neuron(Neuron* neuron, uint8_t type);
void build_connections(Neuron* neural_network, Gene* genome, int genome_length, int neuron_count);
Neuron* find_neuron_by_id(Neuron* neural_network, int neuron_count, uint16_t id);
float apply_activation_function(float x, uint8_t activation_function);
void propagate_signal_from_neuron(NeuronID id, NeuralNetwork* net, bool* visited);
NeuralNetwork* initialize_neural_network(Gene* genome, int genome_length);
void propagate_signal(NeuralNetwork* network);
const char* activation_function_to_string(ActivationFunctionType type);
const char* neuron_id_to_string(NeuronID id);
const char* neuron_type_to_string(NeuronType type);

#endif // NEURON_ENCODING_H
