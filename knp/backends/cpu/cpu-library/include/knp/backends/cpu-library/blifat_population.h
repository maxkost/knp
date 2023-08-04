/**
 * @file blifat_population.h
 * @brief BLIFAT neurons calculation routines definition.
 * @author Artiom N.
 * @date 21.02.2023
 */

#pragma once

#include <knp/core/message_bus.h>
#include <knp/core/population.h>
#include <knp/neuron-traits/blifat.h>

#include <mutex>
#include <optional>
#include <queue>
#include <vector>

/**
 * @brief Make one execution step for a population of BLIFAT neurons.
 * @param population population to update.
 * @param endpoint message endpoint used for message exchange.
 * @param step_n execution step.
 */
void calculate_blifat_population(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::MessageEndpoint &endpoint,
    size_t step_n);


/**
 * @brief Make one execution step for a population of BLIFAT neurons.
 * @param population population to update.
 * @param endpoint message endpoint used for message exchange.
 * @param step_n execution step.
 * @param m mutex.
 */
void calculate_blifat_population(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::MessageEndpoint &endpoint,
    size_t step_n, std::mutex &m);


/**
 * @brief partial pre impact population calculation.
 * @param population the population to be updated.
 * @param part_start first neuron index to be calculated.
 * @param part_size number of neurons to be calculated in a single call.
 * @note Used for parallelization.
 */
void calculate_neurons_state_part(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, size_t part_start, size_t part_size);


/**
 * @brief Process messages sent to the current population.
 * @param population the population to be updated.
 * @param messages synaptic impacts sent to the population.
 * @note Used for parallelization. See later if this function serves as a bottleneck.
 */
void process_inputs(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population,
    const std::vector<knp::core::messaging::SynapticImpactMessage> &messages);


/**
 * @brief Partially calculate neurons states after impact.
 * @param population the population to be updated.
 * @param message output spike message that this function updates.
 * @param part_start first neuron index to be updated.
 * @param part_size number of neurons to be updated in a single call.
 * @param mutex mutex that is locked to update a message.
 * @note This function is used for parallelization.
 */
void calculate_neurons_post_input_state_part(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::messaging::SpikeMessage &message,
    size_t part_start, size_t part_size, std::mutex &mutex);
