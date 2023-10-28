#ifndef GENE_ENCODING_H
#define GENE_ENCODING_H

#include <stdint.h>

#define NUM_NEURONS 20
#define NUM_ACTIVATION_FUNCTIONS 3

// Structure to hold gene details
typedef struct {
    uint64_t gene;         // 64 bits for the gene
    /* 
    First two bits is for the input type
        00 - 0: Sensory input
        01 - 1: Hidden input
        10 - 2: Constant input
        11 - 3: Reserved for future use
    Next 10 bits is for the source neuron ID
        Takes values from 0 to 1023 % num_neurons
    Next 2 bits is for the output type
        00 - 0: Action output
        01 - 1: Hidden output
        10 - 2: Reserved for future use
        11 - 3: Reserved for future use
    Next 10 bits is for the destination neuron ID
        Takes values from 0 to 1023 % num_neurons
    Next 24 bits is for the weight, signed
        Takes values from -8388608 to 8388607 / 2097151
        Roughly -4 to 4
    Next 8 bits is for the activation function
        Takes values from 0 to 255 % num_activation_functions
    Next 8 bits is reserved for future use
    */

} Gene;

uint8_t get_input_type(const Gene* gene);
uint16_t get_source_neuron_id(const Gene* gene);
uint8_t get_output_type(const Gene* gene);
uint16_t get_destination_neuron_id(const Gene* gene);
float get_weight(const Gene* gene);
uint8_t get_activation_function(const Gene* gene);

#endif // GENE_ENCODING_H
