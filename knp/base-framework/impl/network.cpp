/**
 * @file network.cpp
 * @brief Network class implementation.
 * @author Artiom N.
 * @date 22.03.2023
 */

#include <knp/framework/network.h>

#include <spdlog/spdlog.h>

#include <algorithm>

#include <boost/preprocessor.hpp>


namespace knp::framework
{

Network::PopulationIterator Network::begin_populations()
{
    return populations_.begin();
}


Network::PopulationConstIterator Network::begin_populations() const
{
    return populations_.cbegin();
}


Network::PopulationIterator Network::end_populations()
{
    return populations_.end();
}


Network::PopulationConstIterator Network::end_populations() const
{
    return populations_.cend();
}


Network::ProjectionIterator Network::begin_projections()
{
    return projections_.begin();
}


Network::ProjectionConstIterator Network::begin_projections() const
{
    return projections_.cbegin();
}


Network::ProjectionIterator Network::end_projections()
{
    return projections_.end();
}


Network::ProjectionConstIterator Network::end_projections() const
{
    return projections_.cend();
}


template <typename VT>
typename std::vector<VT>::iterator find_variant(const knp::core::UID &uid, std::vector<VT> &container)
{
    auto result = std::find_if(
        container.begin(), container.end(),
        [&uid](VT &p_variant) -> bool
        { return std::visit([&uid](auto &var_val) { return var_val.get_uid() == uid; }, p_variant); });
    return result;
}


template <typename VT>
typename std::vector<VT>::const_iterator find_cvariant(const knp::core::UID &uid, const std::vector<VT> &container)
{
    auto result = std::find_if(
        container.cbegin(), container.cend(),
        [&uid](const VT &p_variant) -> bool
        { return std::visit([&uid](const auto &var_val) { return var_val.get_uid() == uid; }, p_variant); });
    return result;
}


void Network::add_population(core::AllPopulationsVariant &&population)
{
    SPDLOG_DEBUG("Add population variant");
    populations_.emplace_back(population);
}


template <typename PopulationType>
void Network::add_population(typename std::decay<PopulationType>::type &&population)
{
    SPDLOG_DEBUG("Add population");
    add_population(population);
}


template <typename PopulationType>
void Network::add_population(typename std::decay<PopulationType>::type &population)
{
    SPDLOG_DEBUG("Add population");
    add_population(population);
}


template <typename PopulationType>
PopulationType &Network::get_population(const knp::core::UID &population_uid)
{
    static_assert(
        boost::mp11::mp_contains<AllPopulations, PopulationType>(),
        "This population type doesn't supported by the Network class! Add type to the population types list.");

    SPDLOG_DEBUG("Get population {}", std::string(population_uid));
    if (auto pop_iterator = find_variant<core::AllPopulationsVariant>(population_uid, populations_);
        pop_iterator != populations_.end())
    {
        return std::get<PopulationType>(*pop_iterator);
    }
    throw std::runtime_error("Can't find population with uid = " + std::string(population_uid) + "!");
}


template <typename PopulationType>
const PopulationType &Network::get_population(const knp::core::UID &population_uid) const
{
    SPDLOG_DEBUG("Get population {}", std::string(population_uid));
    if (const auto pop_iterator = find_cvariant<core::AllPopulationsVariant>(population_uid, populations_);
        pop_iterator != populations_.cend())
    {
        return std::get<PopulationType>(*pop_iterator);
    }
    throw std::runtime_error("Can't find population with uid = " + std::string(population_uid) + "!");
}


core::AllPopulationsVariant &Network::get_population(const knp::core::UID &population_uid)
{
    SPDLOG_DEBUG("Get projection {}", std::string(population_uid));

    auto iter = find_variant<core::AllPopulationsVariant>(population_uid, populations_);

    if (populations_.end() == iter)
    {
        throw std::runtime_error("Can't find population with uid = " + std::string(population_uid) + "!");
    }

    return *iter;
}


void Network::remove_population(const core::UID &population_uid)
{
    SPDLOG_DEBUG("Remove population with uid {}", std::string(population_uid));
    auto result = find_variant<core::AllPopulationsVariant>(population_uid, populations_);

    if (result == populations_.end())
    {
        throw std::runtime_error("Can't remove not existed population with uid = " + std::string(population_uid) + "!");
    }
    populations_.erase(result);
}


void Network::add_projection(core::AllProjectionsVariant &&projection)
{
    SPDLOG_DEBUG("Add projection variant");
    projections_.emplace_back(projection);
}


template <typename ProjectionType>
void Network::add_projection(typename std::decay<ProjectionType>::type &&projection)
{
    SPDLOG_DEBUG("Add projection [move] {}", std::string(projection.get_uid()));
    projections_.emplace_back(std::move(projection));
}


template <typename ProjectionType>
void Network::add_projection(typename std::decay<ProjectionType>::type &projection)
{
    SPDLOG_DEBUG("Add projection [copy] {}", std::string(projection.get_uid()));
    projections_.emplace_back(projection);
}


template <typename SynapseType>
void Network::add_projection(
    knp::core::UID projection_uid, knp::core::UID pre_population_uid, knp::core::UID post_population_uid,
    typename knp::core::Projection<SynapseType>::SynapseGenerator generator, size_t synapses_count)
{
    SPDLOG_DEBUG("Add projection {}", std::string(projection_uid));
    add_projection(core::AllProjectionsVariant(knp::core::Projection<SynapseType>(
        projection_uid, pre_population_uid, post_population_uid, generator, synapses_count)));
}


template <typename ProjectionType>
ProjectionType &Network::get_projection(const knp::core::UID &projection_uid)
{
    static_assert(
        boost::mp11::mp_contains<core::AllProjections, ProjectionType>(),
        "This projection type doesn't supported by the Network class! Add type to the projection types list.");

    SPDLOG_DEBUG("Get projection {}", std::string(projection_uid));

    auto proj_iterator = find_variant<core::AllProjectionsVariant>(projection_uid, projections_);
    if (proj_iterator != projections_.end())
    {
        return std::get<ProjectionType>(*proj_iterator);
    }

    throw std::runtime_error("Can't find projection with uid = " + std::string(projection_uid) + "!");
}


template <typename ProjectionType>
const ProjectionType &Network::get_projection(const knp::core::UID &projection_uid) const
{
    SPDLOG_DEBUG("Get projection {}", std::string(projection_uid));

    auto proj_iterator = find_cvariant<core::AllProjectionsVariant>(projection_uid, projections_);
    if (proj_iterator != projections_.cend())
    {
        return std::get<ProjectionType>(*proj_iterator);
    }

    throw std::runtime_error("Can't find projection with uid = " + std::string(projection_uid) + "!");
}


core::AllProjectionsVariant &Network::get_projection(const knp::core::UID &projection_uid)
{
    SPDLOG_DEBUG("Get projection {}", std::string(projection_uid));

    auto iter = find_variant<core::AllProjectionsVariant>(projection_uid, projections_);

    if (projections_.end() == iter)
    {
        throw std::runtime_error("Can't find projection with uid = " + std::string(projection_uid) + "!");
    }

    return *iter;
}


void Network::remove_projection(const core::UID &projection_uid)
{
    SPDLOG_DEBUG("Remove projection with uid {}", std::string(projection_uid));
    auto result = find_variant<core::AllProjectionsVariant>(projection_uid, projections_);

    if (result == projections_.end())
    {
        throw std::runtime_error("Can't remove not existed projection with uid = " + std::string(projection_uid) + "!");
    }
    projections_.erase(result);
}

namespace nt = knp::neuron_traits;

#define INSTANCE_POPULATION_FUNCTIONS(n, template_for_instance, neuron_type)                                      \
    template KNP_DECLSPEC void Network::add_population<knp::core::Population<nt::neuron_type>>(                   \
        knp::core::Population<nt::neuron_type> &&);                                                               \
    template KNP_DECLSPEC void Network::add_population<knp::core::Population<nt::neuron_type>>(                   \
        knp::core::Population<nt::neuron_type> &);                                                                \
    template KNP_DECLSPEC knp::core::Population<nt::neuron_type>                                                  \
        &Network::get_population<knp::core::Population<knp::neuron_traits::neuron_type>>(const knp::core::UID &); \
    template KNP_DECLSPEC const knp::core::Population<nt::neuron_type> &                                          \
    Network::get_population<knp::core::Population<knp::neuron_traits::neuron_type>>(const knp::core::UID &) const;

namespace st = knp::synapse_traits;

#define INSTANCE_PROJECTION_FUNCTIONS(n, template_for_instance, synapse_type)                      \
    template KNP_DECLSPEC void Network::add_projection<knp::core::Projection<st::synapse_type>>(   \
        knp::core::Projection<st::synapse_type> &&);                                               \
    template KNP_DECLSPEC void Network::add_projection<knp::core::Projection<st::synapse_type>>(   \
        knp::core::Projection<st::synapse_type> &);                                                \
    template KNP_DECLSPEC void Network::add_projection<st::synapse_type>(                          \
        knp::core::UID, knp::core::UID, knp::core::UID,                                            \
        typename knp::core::Projection<st::synapse_type>::SynapseGenerator, size_t);               \
    template KNP_DECLSPEC knp::core::Projection<st::synapse_type>                                  \
        &Network::get_projection<knp::core::Projection<st::synapse_type>>(const knp::core::UID &); \
    template KNP_DECLSPEC const knp::core::Projection<st::synapse_type>                            \
        &Network::get_projection<knp::core::Projection<st::synapse_type>>(const knp::core::UID &) const;

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_POPULATION_FUNCTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_NEURONS))
// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_PROJECTION_FUNCTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_SYNAPSES))

}  // namespace knp::framework
