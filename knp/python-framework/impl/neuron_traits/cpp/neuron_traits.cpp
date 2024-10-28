/**
 * @file neuron_traits.cpp
 * @brief Python bindings for neuron traits.
 * @author Artiom N.
 * @date 20.02.24
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include "common.h"

BOOST_PYTHON_MODULE(KNP_FULL_LIBRARY_NAME)
{
#define _KNP_IN_NEURON_TRAITS

#include "blifat.cpp"                // NOLINT
#include "resource_stdp_blifat.cpp"  // NOLINT

#undef _KNP_IN_NEURON_TRAITS
}
