#include "neuron_encoding.h"
#include "gene_encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Initialize a neural network from a genome
NeuralNetwork* initialize_neural_network(Gene* genome, int genome_length) {
    NeuralNetwork* network = malloc(sizeof(NeuralNetwork));
    if (!network) {
        return NULL;  // Allocation failed
    }

    network->num_sensory_neurons = 0;
    network->num_output_neurons = 0;
    network->sensory_ids = malloc(genome_length * sizeof(uint16_t));  // Max possible size
    network->output_ids = malloc(genome_length * sizeof(uint16_t));  // Max possible size

    if (!network->sensory_ids || !network->output_ids) {
        free(network);
        return NULL;  // Allocation failed
    }

    // Dynamic array to keep track of unique neuron IDs
    uint16_t* unique_neurons = malloc(genome_length * 2 * sizeof(uint16_t));
    if (!unique_neurons) {
        free(network->sensory_ids);
        free(network->output_ids);
        free(network);
        return NULL;  // Allocation failed
    }

    int unique_count = 0;
    Neuron* neurons = malloc(genome_length * 2 * sizeof(Neuron));
    if (!neurons) {
        free(unique_neurons);
        free(network->sensory_ids);
        free(network->output_ids);
        free(network);
        return NULL;  // Allocation failed
    }

    int neuron_count = 0;

    // Iterate through each gene in the genome
    for (int i = 0; i < genome_length; ++i) {
        uint16_t source_id = get_source_neuron_id(&genome[i]);
        uint16_t dest_id = get_destination_neuron_id(&genome[i]);

        if (source_id == 0xFFFF || dest_id == 0xFFFF) {
            // Handle error or reserved types
            continue;
        }

        int is_source_new = is_unique_id(unique_neurons, &unique_count, source_id);
        int is_dest_new = is_unique_id(unique_neurons, &unique_count, dest_id);
        if (is_source_new) {
            initialize_neuron(&neurons[neuron_count], get_input_type(&genome[i]));
            if (get_input_type(&genome[i]) == SENSORY) {
                network->sensory_ids[network->num_sensory_neurons] = source_id;
                network->num_sensory_neurons++;
            }
            neurons[neuron_count].id = source_id;  // Setting the ID
            neuron_count++;
        }
        if (is_dest_new) {
            initialize_neuron(&neurons[neuron_count], get_output_type(&genome[i]));
            if (get_output_type(&genome[i]) == OUTPUT) {
                network->output_ids[network->num_output_neurons] = dest_id;
                network->num_output_neurons++;
            }
            neurons[neuron_count].id = dest_id;  // Setting the ID
            neuron_count++;
        }
    }

    // Set up connections
    build_connections(neurons, genome, genome_length, neuron_count);

    // Cleanup
    free(unique_neurons);

    // Reallocate neurons and ID arrays to actual sizes
    neurons = realloc(neurons, neuron_count * sizeof(Neuron));
    network->sensory_ids = realloc(network->sensory_ids, network->num_sensory_neurons * sizeof(uint16_t));
    network->output_ids = realloc(network->output_ids, network->num_output_neurons * sizeof(uint16_t));

    if (!neurons || !network->sensory_ids || !network->output_ids) {
        // Handle realloc failure
        return NULL;
    }

    network->neurons = neurons;
    network->total_neurons = neuron_count;

    return network;
}


// Build connections between neurons
void build_connections(Neuron* neural_network, Gene* genome, int genome_length, int neuron_count) {
    // Iterate through each gene in the genome
    for (int i = 0; i < genome_length; ++i) {
        uint16_t source_id = get_source_neuron_id(&genome[i]);
        uint16_t dest_id = get_destination_neuron_id(&genome[i]);\

        if (source_id == 0xFFFF || dest_id == 0xFFFF) {
            // Handle error or reserved types
            continue;
        }

        // Find the source and destination neurons
        Neuron* source_neuron = find_neuron_by_id(neural_network, neuron_count, source_id);

        

        if (source_neuron) {  // Check if the pointers are valid
            source_neuron->num_connections++;
            Connection* new_connections = realloc(source_neuron->connections, source_neuron->num_connections * sizeof(Connection));
            if (new_connections) {
                source_neuron->connections = new_connections;
                source_neuron->connections[source_neuron->num_connections - 1].id = dest_id;
                source_neuron->connections[source_neuron->num_connections - 1].weight = get_weight(&genome[i]);
                source_neuron->connections[source_neuron->num_connections - 1].activation_function = get_activation_function(&genome[i]);
            } else {
                // Handle realloc failure, for example:
                source_neuron->num_connections--;  // Rollback the increment
            }
        }
    }
}

// Helper function to check if an ID is unique
int is_unique_id(uint16_t* unique_neurons, int* unique_count, uint16_t id) {
    for (int i = 0; i < *unique_count; ++i) {
        if (unique_neurons[i] == id) {
            return 0;
        }
    }
    unique_neurons[*unique_count] = id;
    (*unique_count)++;
    return 1;
}

// Helper function to initialize a neuron
void initialize_neuron(Neuron* neuron, uint8_t type) {
    neuron->type = type;
    neuron->data = 0;
    neuron->connections = NULL;
    neuron->activation_threshold = 0;
    neuron->num_connections = 0;
}

// Helper function to find a neuron by ID
Neuron* find_neuron_by_id(Neuron* neural_network, int neuron_count, uint16_t id) {
    for (int i = 0; i < neuron_count; ++i) {
        if (neural_network[i].id == id) {
            return &neural_network[i];
        }
    }
    return NULL;
}

// Activation function implementations
float relu(float x) {
    return x > 0 ? x : 0;
}

float sigmoid(float x) {
    return 1 / (1 + exp(-x));
}

float tanh_activation(float x) {
    return tanh(x);
}

// Activation function dispatcher
float apply_activation_function(float x, uint8_t activation_function) {
    switch (activation_function) {
        case RELU:
            return relu(x);
        case SIGMOID:
            return sigmoid(x);
        case TANH:
            return tanh_activation(x);
        default:
            return x;  // Identity function as default (no activation)
    }
}

// Updated recursive function to propagate signal from a given neuron
void propagate_signal_from_neuron(NeuronID id, NeuralNetwork* net, bool* visited) {
    // Find the neuron
    Neuron* neuron = find_neuron_by_id(net->neurons, net->total_neurons, id);
    if (!neuron) {
        return;
    }
    int neuron_index = (int)(neuron - net->neurons);
    // Base case: if the neuron has no outgoing connections or is already visited, return
    if (neuron->num_connections == 0 || visited[neuron_index]) {
        return;
    }

    // Mark the current neuron as visited
    visited[neuron_index] = true;

    // Propagate signal through all connections
    for (int i = 0; i < neuron->num_connections; ++i) {
        // Use activation function
        float activated_output = apply_activation_function(neuron->data, neuron->connections[i].activation_function);

        // Update the connected neuron's data
        Neuron* target = find_neuron_by_id(net->neurons, net->total_neurons, neuron->connections[i].id);
        if (target) {
            target->data += neuron->connections[i].weight * activated_output;
            // Recursively propagate signal from the connected neuron
            propagate_signal_from_neuron(neuron->connections[i].id, net, visited);
        }
    }

    // Unmark the current neuron as visited for future calls
    visited[neuron_index] = false;

}

// Main function to initiate signal propagation from sensory neurons
void propagate_signal(NeuralNetwork* network) {
    // Create a visited array
    bool visited[network->total_neurons];
    memset(visited, 0, sizeof(visited));

    for (int i = 0; i < network->num_sensory_neurons; ++i) {
        propagate_signal_from_neuron(network->sensory_ids[i], network, visited);
    }
}


// Function to return the string name for ActivationFunctionType enum
const char* activation_function_to_string(ActivationFunctionType type) {
    switch(type) {
        case RELU: return "RELU";
        case SIGMOID: return "SIGMOID";
        case TANH: return "TANH";
        default: return "UNKNOWN";
    }
}

// Function to return the string name for NeuronID enum
const char* neuron_id_to_string(NeuronID id) {
    switch(id) {
        case L_n: return "L_n";
        case L_ne: return "L_ne";
        case L_e: return "L_e";
        case L_se: return "L_se";
        case L_s: return "L_s";
        case L_sw: return "L_sw";
        case L_w: return "L_w";
        case L_nw: return "L_nw";
        
        case LW_n: return "LW_n";
        case LW_ne: return "LW_ne";
        case LW_e: return "LW_e";
        case LW_se: return "LW_se";
        case LW_s: return "LW_s";
        case LW_sw: return "LW_sw";
        case LW_w: return "LW_w";
        case LW_nw: return "LW_nw";
        
        case I_0: return "I_0";
        case I_1: return "I_1";
        case I_2: return "I_2";
        case I_3: return "I_3";
        case I_4: return "I_4";
        
        case M_n: return "M_n";
        case M_ne: return "M_ne";
        case M_e: return "M_e";
        case M_se: return "M_se";
        case M_s: return "M_s";
        case M_sw: return "M_sw";
        case M_w: return "M_w";
        case M_nw: return "M_nw";
        
        case M_r: return "M_r";
        
        case TOTAL_NEURONS: return "TOTAL_NEURONS";
        
        default: return "UNKNOWN";
    }
}

// Function to return the string name for NeuronType enum
const char* neuron_type_to_string(NeuronType type) {
    switch(type) {
        case SENSORY: return "SENSORY";
        case INTERNAL: return "INTERNAL";
        case CONSTANT: return "CONSTANT";
        case OUTPUT: return "OUTPUT";
        default: return "UNKNOWN";
    }
}
// ... other utility functions as needed
