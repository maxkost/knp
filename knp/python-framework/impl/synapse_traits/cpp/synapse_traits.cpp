/**
 * @file synapse_traits.cpp
 * @brief Neurons properties Python bindings.
 * @author Artiom N.
 * @date 21.02.24
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include "common.h"

BOOST_PYTHON_MODULE(KNP_FULL_LIBRARY_NAME)
{
#define _KNP_IN_SYNAPSE_TRAITS

#include "delta.cpp"  // NOLINT

#undef _KNP_IN_SYNAPSE_TRAITS
}
