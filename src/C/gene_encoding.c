#include "gene_encoding.h"
#include <stdint.h>

// Extracts the input type from the gene
uint8_t get_input_type(const Gene* gene) {
    // Use bit manipulation to extract the first 2 bits
    uint8_t input_type = gene->gene >> 62;
    return input_type;
}

// Extracts the source neuron ID from the gene
uint16_t get_source_neuron_id(const Gene* gene) {
    // Use bit manipulation to extract the next 10 bits
    uint16_t bit_source_id = gene->gene >> 52;
    bit_source_id = bit_source_id & 0x3FF;
    uint16_t num_neurons, offset;
    get_source_num_corresponding_neurons_and_offset(gene, &num_neurons, &offset);
    // Handle error or reserved types
    if (num_neurons == 0xFFFF || offset == 0xFFFF) {
        return 0xFFFF;
    }
    uint16_t source_id = (bit_source_id % num_neurons) + offset;
    return source_id;
}

// Extracts the output type from the gene
uint8_t get_output_type(const Gene* gene) {
    // Use bit manipulation to extract the next 2 bits
    uint8_t output_type = gene->gene >> 50;
    output_type = output_type & 0x3;
    return output_type;
}

// Extracts the destination neuron ID from the gene
uint16_t get_destination_neuron_id(const Gene* gene) {
    // Use bit manipulation to extract the next 10 bits
    uint16_t bit_output_id = gene->gene >> 40;
    bit_output_id = bit_output_id & 0x3FF;
    // Convert the 10 bits by taking the modulus with respect to the constant number of neurons
    uint16_t num_neurons, offset;
    get_output_num_corresponding_neurons_and_offset(gene, &num_neurons, &offset);
    // Handle error or reserved types
    if (num_neurons == 0xFFFF || offset == 0xFFFF) {
        return 0xFFFF;
    }
    uint16_t output_id = (bit_output_id % num_neurons) + offset;
    return output_id;
}

// Extracts the weight from the gene
float get_weight(const Gene* gene) {
    // Use bit manipulation to extract the next 24 bits
    uint32_t int_weight = gene->gene >> 16;
    int_weight = int_weight & 0xFFFFFF;
    // Convert the integer to a float by dividing by 2^21
    float float_weight = (((float) int_weight) - 8388608)/ 2097152;
    return float_weight;
}

// Extracts the activation function type from the gene
uint8_t get_activation_function(const Gene* gene) {
    // Use bit manipulation to extract the next 8 bits
    uint8_t bit_activation = gene->gene >> 8;
    bit_activation = bit_activation & 0xFF;
    // Convert the 10 bits to a signed integer by taking the modulus with respect to the constant number of neurons
    uint8_t activation = bit_activation % NUM_ACTIVATION_FUNCTIONS;
    return activation;
}

// Helper method to get the number of corresponding source neurons and offset based on the input type
void get_source_num_corresponding_neurons_and_offset(const Gene* gene, uint16_t* num_neurons, uint16_t* offset) {
    uint8_t input_type = get_input_type(gene);
    switch(input_type) {
        case 0:  // Sensory input
            *num_neurons = NUM_SENSORY_NEURONS;
            *offset = 0;
            break;
        case 1:  // Hidden input
            *num_neurons = NUM_INTERNAL_NEURONS;
            *offset = NUM_SENSORY_NEURONS;
            break;
        case 2:  // Constant input
            *num_neurons = NUM_CONSTANT_NEURONS;
            *offset = NUM_SENSORY_NEURONS + NUM_INTERNAL_NEURONS;
            break;
        default:
            *num_neurons = 0xFFFF;
            *offset = 0xFFFF;  // Handle error or reserved types
    }
}

// Helper method to get the number of corresponding source neurons and offset based on the output type
void get_output_num_corresponding_neurons_and_offset(const Gene* gene, uint16_t* num_neurons, uint16_t* offset) {
    uint8_t output_type = get_output_type(gene);  // Changed from input_type to output_type
    switch(output_type) {
        case 3:  // Action output
            *num_neurons = NUM_ACTION_NEURONS;
            *offset = NUM_SENSORY_NEURONS + NUM_INTERNAL_NEURONS + NUM_CONSTANT_NEURONS;
            break;
        case 1:  // Hidden output
            *num_neurons = NUM_INTERNAL_NEURONS;
            *offset = NUM_SENSORY_NEURONS;
            break;
        default:
            *num_neurons = 0xFFFF;
            *offset = 0xFFFF;  // Handle error or reserved types
    }
}
