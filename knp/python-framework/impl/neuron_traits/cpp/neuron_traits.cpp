/** @file neuron_traits.cpp
 * @brief Neurons properties Python bindings.
 * @author Artiom N.
 * @date 20.02.24
 */

#include "common.h"

BOOST_PYTHON_MODULE(KNP_FULL_LIBRARY_NAME)
{
#define _KNP_IN_NEURON_TRAITS

#include "blifat.cpp"  // NOLINT

#undef _KNP_IN_NEURON_TRAITS
}
