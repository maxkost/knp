/**
 * @file population.cpp
 * @brief Population class implementation.
 * @author Andrey V.
 * @date 26.01.2023
 */

#include <knp/core/projection.h>


namespace knp::core
{

template <typename SynapseType>
knp::core::Projection<SynapseType>::Projection(UID presynaptic_uid, UID postsynaptic_uid)
    : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
}


template <typename SynapseType>
knp::core::Projection<SynapseType>::Projection(UID uid, UID presynaptic_uid, UID postsynaptic_uid)
    : base_{uid}, presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
}


template <typename SynapseType>
knp::core::Projection<SynapseType>::Projection(
    UID presynaptic_uid, UID postsynaptic_uid, const SynapseGenerator &generator, size_t num_iterations)
    : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    for (size_t i = 0; i < num_iterations; ++i)
    {
        if (auto params = generator(i))
        {
            parameters_.emplace_back(std::move(params.value()));
        }
    }
}


template <typename SynapseType>
knp::core::Projection<SynapseType>::Projection(
    UID presynaptic_uid, UID postsynaptic_uid, const SynapseGenerator1 &generator, size_t synapses_count)
    : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    for (size_t i = 0; i < synapses_count; ++i)
    {
        if (auto params = generator(i))
        {
            auto &&[p, id_from, id_to] = std::move(params.value());
            parameters_.emplace_back(Synapse{std::move(p), id_from, id_to});
        }
    }
}


template <typename SynapseType>
knp::core::Projection<SynapseType>::Projection(
    UID uid, UID presynaptic_uid, UID postsynaptic_uid, const SynapseGenerator1 &generator, size_t synapses_count)
    : base_{uid}, presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    for (size_t i = 0; i < synapses_count; ++i)
    {
        if (auto params = generator(i))
        {
            auto &&[p, id_from, id_to] = std::move(params.value());
            parameters_.emplace_back(Synapse{std::move(p), id_from, id_to});
        }
    }
}


template <typename SynapseType>
std::vector<std::tuple<size_t, size_t, size_t>> knp::core::Projection<SynapseType>::get_connections() const
{
    std::vector<std::tuple<size_t, size_t, size_t>> result;
    result.reserve(parameters_.size());
    for (size_t i = 0; i < parameters_.size(); ++i)
    {
        result.emplace_back(std::make_tuple(parameters_[i].id_from, i, parameters_[i].id_to));
    }
    return result;
}


template <typename SynapseType>
size_t knp::core::Projection<SynapseType>::add_synapses(size_t num_iterations, const SynapseGenerator &generator)
{
    const size_t starting_size = parameters_.size();
    for (size_t i = 0; i < num_iterations; ++i)
    {
        if (auto data = generator(i))
        {
            parameters_.emplace_back(std::move(data.value()));
        }
    }
    return parameters_.size() - starting_size;
}


template <typename SynapseType>
size_t knp::core::Projection<SynapseType>::add_synapses(const std::vector<Synapse> &synapses)
{
    const size_t starting_size = parameters_.size();
    std::copy(synapses.begin(), synapses.end(), std::back_insert_iterator(parameters_));
    return parameters_.size() - starting_size;
}


#define INSTANCE_PROJECTIONS(n, template_for_instance, synapse_type) template class knp::core::Projection<synapse_type>;

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_PROJECTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_SYNAPSES))

}  // namespace knp::core
