/**
 * @file population.cpp
 * @brief Population class implementation.
 * @author Andrey V.
 * @date 26.01.2023
 */

#include <knp/core/projection.h>

#include <spdlog/spdlog.h>


namespace knp::core
{

template <typename SynapseType>
Projection<SynapseType>::Projection(UID presynaptic_uid, UID postsynaptic_uid)
    : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    SPDLOG_DEBUG(
        "Creating projection with UID = {}, presynaptic UID = {}, postsynaptic UID = {}", std::string(get_uid()),
        std::string(presynaptic_uid_), std::string(postsynaptic_uid_));
}


template <typename SynapseType>
Projection<SynapseType>::Projection(UID uid, UID presynaptic_uid, UID postsynaptic_uid)
    : base_{uid}, presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    SPDLOG_DEBUG(
        "Creating projection with UID = {}, presynaptic UID = {}, postsynaptic UID = {}", std::string(get_uid()),
        std::string(presynaptic_uid_), std::string(postsynaptic_uid_));
}


template <typename SynapseType>
Projection<SynapseType>::Projection(
    UID presynaptic_uid, UID postsynaptic_uid, const SynapseGenerator &generator, size_t num_iterations)
    : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    SPDLOG_DEBUG(
        "Creating projection with UID = {}, presynaptic UID = {}, postsynaptic UID = {}, i = {}",
        std::string(get_uid()), std::string(presynaptic_uid_), std::string(postsynaptic_uid_), num_iterations);
    for (size_t i = 0; i < num_iterations; ++i)
    {
        if (auto params = generator(i))
        {
            parameters_.emplace_back(std::move(params.value()));
        }
    }
}


template <typename SynapseType>
Projection<SynapseType>::Projection(
    UID presynaptic_uid, UID postsynaptic_uid, const SynapseGenerator1 &generator, size_t num_iterations)
    : presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    SPDLOG_DEBUG(
        "Creating projection with UID = {}, presynaptic UID = {}, postsynaptic UID = {}, i = {}",
        std::string(get_uid()), std::string(presynaptic_uid_), std::string(postsynaptic_uid_), num_iterations);
    for (size_t i = 0; i < num_iterations; ++i)
    {
        if (auto params = generator(i))
        {
            auto &&[p, id_from, id_to] = std::move(params.value());
            parameters_.emplace_back(Synapse{std::move(p), id_from, id_to});
        }
    }
}


template <typename SynapseType>
Projection<SynapseType>::Projection(
    UID uid, UID presynaptic_uid, UID postsynaptic_uid, const SynapseGenerator1 &generator, size_t num_iterations)
    : base_{uid}, presynaptic_uid_(presynaptic_uid), postsynaptic_uid_(postsynaptic_uid)
{
    SPDLOG_DEBUG(
        "Creating projection with UID = {}, presynaptic UID = {}, postsynaptic UID = {}, i = {}",
        std::string(get_uid()), std::string(presynaptic_uid_), std::string(postsynaptic_uid_), num_iterations);
    for (size_t i = 0; i < num_iterations; ++i)
    {
        if (auto params = generator(i))
        {
            auto &&[p, id_from, id_to] = std::move(params.value());
            parameters_.emplace_back(Synapse{std::move(p), id_from, id_to});
        }
    }
}


template <typename SynapseType>
std::vector<size_t> knp::core::Projection<SynapseType>::get_by_presynaptic_neuron(size_t neuron_index) const
{
    std::vector<size_t> res;

    for (size_t i = 0; i < parameters_.size(); ++i)
    {
        if (parameters_[i].id_from_ == neuron_index)
        {
            res.push_back(i);
        }
    }
    return res;
}


template <typename SynapseType>
std::vector<size_t> knp::core::Projection<SynapseType>::get_by_postsynaptic_neuron(size_t neuron_index) const
{
    std::vector<size_t> res;

    for (size_t i = 0; i < parameters_.size(); ++i)
    {
        if (parameters_[i].id_to_ == neuron_index)
        {
            res.push_back(i);
        }
    }
    return res;
}


template <typename SynapseType>
std::vector<std::tuple<size_t, size_t, size_t>> knp::core::Projection<SynapseType>::get_connections() const
{
    std::vector<std::tuple<size_t, size_t, size_t>> result;
    result.reserve(parameters_.size());
    for (size_t i = 0; i < parameters_.size(); ++i)
    {
        result.emplace_back(std::make_tuple(parameters_[i].id_from_, i, parameters_[i].id_to_));
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


template <typename SynapseType>
size_t knp::core::Projection<SynapseType>::disconnect_postsynaptic_neuron(size_t neuron_index)
{
    return disconnect_if([neuron_index](const Synapse &synapse) { return synapse.id_to_ == neuron_index; });
}


template <typename SynapseType>
size_t knp::core::Projection<SynapseType>::disconnect_presynaptic_neuron(size_t neuron_index)
{
    return disconnect_if([neuron_index](const Synapse &synapse) { return synapse.id_from_ == neuron_index; });
}


template <typename SynapseType>
size_t knp::core::Projection<SynapseType>::disconnect_neurons(size_t neuron_from, size_t neuron_to)
{
    return disconnect_if([neuron_from, neuron_to](const Synapse &synapse)
                         { return (synapse.id_from_ == neuron_from) && (synapse.id_to_ == neuron_to); });
}


#define INSTANCE_PROJECTIONS(n, template_for_instance, synapse_type) template class knp::core::Projection<synapse_type>;

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_PROJECTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_SYNAPSES))

}  // namespace knp::core
