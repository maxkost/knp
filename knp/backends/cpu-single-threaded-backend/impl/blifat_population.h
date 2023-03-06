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


void calculate_blifat_population(
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, knp::core::MessageEndpoint &bus);
