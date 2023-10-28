#include "neuron_encoding.h"
#include "gene_encoding.h"
#include <stdlib.h>
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
        uint16_t dest_id = get_destination_neuron_id(&genome[i]);

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
    neuron->activation_threshold = 1;
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
void propagate_signal_from_neuron(NeuronID id, NeuralNetwork* net) {
    // Find the neuron
    Neuron* neuron = find_neuron_by_id(net->neurons, net->total_neurons, id);
    // Base case: if the neuron has no outgoing connections, return
    if (neuron->num_connections == 0) {
        return;
    }

    // Propagate signal through all connections
    for (int i = 0; i < neuron->num_connections; ++i) {
        // Use activation function
        float activated_output = apply_activation_function(neuron->data, neuron->connections[i].activation_function);

        // Update the connected neuron's data
        find_neuron_by_id(net->neurons, net->total_neurons, neuron->connections[i].id)->data += neuron->connections[i].weight * activated_output;

        // Recursively propagate signal from the connected neuron
        propagate_signal_from_neuron(neuron->connections[i].id, net);
    }
}


// Main function to initiate signal propagation from sensory neurons
void propagate_signal(NeuralNetwork* network) {
    for (int i = 0; i < network->num_sensory_neurons; ++i) {
        propagate_signal_from_neuron(network->sensory_ids[i], network);
    }
}


// ... other utility functions as needed
