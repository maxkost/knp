/**
 * @file population.cpp
 * @brief Population class implementation.
 * @author Artiom N.
 * @date 11.01.2023
 */


#include <knp/core/population.h>


namespace knp::core
{

#define INSTANCE_POPULATIONS(n, template_for_instance, neuron_type) template class knp::core::Population<neuron_type>;

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_POPULATIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_NEURONS))

}  // namespace knp::core
