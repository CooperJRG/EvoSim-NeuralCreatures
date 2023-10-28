#include "gene_encoding.h"

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
    // Convert the 10 bits by taking the modulus with respect to the constant number of neurons
    uint16_t source_id = bit_source_id % NUM_NEURONS;
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
    uint16_t output_id = bit_output_id % NUM_NEURONS;
    return output_id;
}

// Extracts the weight from the gene
float get_weight(const Gene* gene) {
    // Use bit manipulation to extract the next 24 bits
    uint32_t int_weight = gene->gene >> 16;
    int_weight = int_weight & 0xFFFFFF;
    // Convert the integer to a float by dividing by 2^21
    float float_weight = (float) int_weight / 2097152;
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