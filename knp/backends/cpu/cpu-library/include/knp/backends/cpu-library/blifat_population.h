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
 * @brief does a single step for a BLIFAT neuron population
 * @param population the population to be updated
 * @param endpoint message endpoint used for message exchange
 * @param step_n current step
 */
void calculate_blifat_population(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::MessageEndpoint &endpoint,
    size_t step_n);


void calculate_blifat_population(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::MessageEndpoint &endpoint,
    size_t step_n, std::mutex &m);
