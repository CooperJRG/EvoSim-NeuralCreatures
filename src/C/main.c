#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>  // for PRIu64
#include "neuron_encoding.h"
#include "gene_encoding.h"

const char* neuron_type_to_string(NeuronType type);
void print_neural_network_structure(NeuralNetwork* network);


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

// Function to print the neural network structure
void print_neural_network_structure(NeuralNetwork* network) {
    printf("Neural Network Structure:\n");
    printf("Total neurons: %d\n", network->total_neurons);
    printf("Sensory neurons: %d\n", network->num_sensory_neurons);
    printf("Output neurons: %d\n", network->num_output_neurons);

    for (int i = 0; i < network->total_neurons; ++i) {
        Neuron* neuron = &network->neurons[i];
        printf("Neuron %d:\n", i);
        printf("\tType: %s\n", neuron_type_to_string(neuron->type));
        printf("\tID: %u\n", neuron->id);
        printf("\tData: %f\n", neuron->data);
        printf("\tActivation Threshold: %f\n", neuron->activation_threshold);
        printf("\tNumber of Connections: %d\n", neuron->num_connections);
        for (int j = 0; j < neuron->num_connections; ++j) {
            printf("\t\tConnection to Neuron ID %u with weight %f\n", neuron->connections[j].id, neuron->connections[j].weight);
        }
    }
}

int main() {
    srand(time(NULL));  // Seed the random number generator

    int genome_length = 1;
    Gene genome[genome_length];

    // Generate a random gene
    genome[0].gene = rand();  // Generate a random long between 0 and 1000
    genome[0].gene = 0x24595655A590200;
    // This numbers first 2 bits are 00, so it is a sensory input
    // The next 10 bits are 0000100100, so the source neuron ID is 36 % 16 = 4
    // The next 2 bits are 01, so it is an internal output
    // The next 10 bits are 0110010101, so the destination neuron ID is 405 % 5 + 16 = 16
    // The next 24 bits are 011001010101101001011001, so the weight is 6642265 / 2097152 = 3.16727876663
    // The next 8 bits are 00000010, so the activation function is 2 % 3 = 2 (tanh)
    // The last 8 bits are 00000000, so they are reserved for future use

    // Initialize the neural network
    NeuralNetwork* network = initialize_neural_network(genome, genome_length);
    if (!network) {
        printf("Failed to initialize neural network.\n");
        return 1;
    }

    // Set initial data on sensory neurons
    for (int i = 0; i < network->num_sensory_neurons; ++i) {
        Neuron* temp_source = find_neuron_by_id(network->neurons, network->total_neurons, network->sensory_ids[i]);
        temp_source->data = rand() / (float)RAND_MAX;  // Random float between 0 and 1
    }
    print_neural_network_structure(network);
    // Propagate the signals through the network
    propagate_signal(network);

    printf("Neural network initialized.\n");
    printf("Genome: %" PRIu64 "\n", genome[0].gene);

    print_neural_network_structure(network);

    // Inspect the data on output neurons
    printf("Output neuron data:\n");
    for (int i = 0; i < network->num_output_neurons; ++i) {
        Neuron* temp_output = find_neuron_by_id(network->neurons, network->total_neurons, network->output_ids[i]);
        printf("Output neuron %d: %f\n", i, temp_output->data);
    }

    // Cleanup
    // Don't forget to free the dynamically allocated memory for the network and its components
    // ...

    return 0;
}
