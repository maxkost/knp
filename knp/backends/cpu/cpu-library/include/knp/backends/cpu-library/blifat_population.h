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
 * @brief Namespace for CPU backends.
 */
namespace knp::backends::cpu
{

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
 * @brief Partially calculate population before it receives synaptic impact messages.
 * @param population population to update.
 * @param part_start index of the first neuron to calculate.
 * @param part_size number of neurons to calculate in a single call.
 * @note The method if used for parallelization.
 */
void calculate_neurons_state_part(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, size_t part_start, size_t part_size);


/**
 * @brief Process messages sent to the current population.
 * @param population population to update.
 * @param messages synaptic impact messages sent to the population.
 * @note The method is used for parallelization. See later if this method serves as a bottleneck.
 */
void process_inputs(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population,
    const std::vector<knp::core::messaging::SynapticImpactMessage> &messages);


/**
 * @brief Partially calculate population after it receives synaptic impact messages.
 * @param population population to update.
 * @param message output spike message to update.
 * @param part_start index of the first neuron to update.
 * @param part_size number of neurons to calculate in a single call.
 * @param mutex mutex that is locked to update a message.
 * @note This method is used for parallelization.
 */
void calculate_neurons_post_input_state_part(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::messaging::SpikeMessage &message,
    size_t part_start, size_t part_size, std::mutex &mutex);

}  // namespace knp::backends::cpu
