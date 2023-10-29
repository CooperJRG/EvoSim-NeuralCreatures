#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>  // for PRIu64
#include "neuron_encoding.h"
#include "gene_encoding.h"

void print_neural_network_structure(NeuralNetwork* network);




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
        printf("\tID: %s\n", neuron_id_to_string(neuron->id));
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

    int genome_length = 8;
    Gene genome[genome_length];

    for (int i = 0; i < genome_length; ++i) {
    uint32_t random1 = rand();  // Generate a random 32-bit integer
    uint32_t random2 = rand();  // Generate another random 32-bit integer

    genome[i].gene = ((uint64_t)random1 << 32) | random2;  // Combine the two random 32-bit integers into one 64-bit integer
    }


    /*
    genome[0].gene = 0x24595655A590200;
       This numbers first 2 bits are 00, so it is a sensory input
       The next 10 bits are 0000100100, so the source neuron ID is 36 % 16 = 4
       The next 2 bits are 01, so it is an internal output
       The next 10 bits are 0110010101, so the destination neuron ID is 405 % 5 + 16 = 16
       The next 24 bits are 011001010101101001011001, so the weight is 6642265 / 2097152 = 3.16727876663
       The next 8 bits are 00000010, so the activation function is 2 % 3 = 2 (tanh)
       The last 8 bits are 00000000, so they are reserved for future use
    */
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

    FILE *neuron_file = fopen("../Python/neurons.csv", "w");
    fprintf(neuron_file, "Index,Type,ID,Label\n");
    
    for (int i = 0; i < network->total_neurons; ++i) {
        Neuron* neuron = &network->neurons[i];
        fprintf(neuron_file, "%d,%s,%u,%s\n", (int)i, neuron_type_to_string(neuron->type), neuron->id, neuron_id_to_string(neuron->id));
    }

    FILE *connection_file = fopen("../Python/connections.csv", "w");
    fprintf(connection_file, "SourceID,TargetID,Weight,ActivationFunction\n");

    for (int i = 0; i < network->total_neurons; ++i) {
        Neuron* neuron = &network->neurons[i];
        for (int j = 0; j < neuron->num_connections; ++j) {
            fprintf(connection_file, "%u,%u,%f,%s\n", neuron->id, neuron->connections[j].id, neuron->connections[j].weight, activation_function_to_string(neuron->connections[j].activation_function));
        }
    }

fclose(connection_file);


fclose(neuron_file);

    // Cleanup
    // Don't forget to free the dynamically allocated memory for the network and its components
    // ...

    return 0;
}
