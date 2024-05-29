/**
 * @file population.cpp
 * @brief Population class implementation.
 * @author Artiom N.
 * @date 11.01.2023
 */


#include <knp/core/population.h>

#include <spdlog/spdlog.h>


namespace knp::core
{
template <typename NeuronType>
Population<NeuronType>::Population(
    Population<NeuronType>::NeuronGenerator generator, size_t neurons_count)  // !OCLint(Parameters used)
{
    SPDLOG_DEBUG("Creating population with UID = {} and neurons count = {}", std::string(get_uid()), neurons_count);
    add_neurons(generator, neurons_count);
}


template <typename NeuronType>
Population<NeuronType>::Population(
    const UID &uid, Population<NeuronType>::NeuronGenerator generator,
    size_t neurons_count)  // !OCLint(Parameters used)
    : base_{uid}
{
    SPDLOG_DEBUG("Creating population with UID = {} and neurons count = {}", std::string(get_uid()), neurons_count);
    add_neurons(generator, neurons_count);
}


#define INSTANCE_POPULATIONS(n, template_for_instance, neuron_type) \
    template class knp::core::Population<knp::neuron_traits::neuron_type>;

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_POPULATIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_NEURONS))

}  // namespace knp::core
