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

#define _KNP_INTERNAL

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


template <typename T, typename VT>
typename std::vector<VT>::iterator Network::find_elem(const knp::core::UID &uid, std::vector<VT> &container)
{
    auto result = std::find_if(
        container.begin(), container.end(),
        [&uid](VT &p_variant) -> bool
        {
            constexpr auto type_n = boost::mp11::mp_find<VT, T>();
            if (p_variant.index() != type_n) return false;
            return uid == (std::get<type_n>(p_variant)).get_uid();
        });
    return result;
}


template <typename Ts, typename VT>
typename std::vector<VT>::iterator Network::find_variant(const knp::core::UID &uid, std::vector<VT> &container)
{
    auto result = std::find_if(
        container.begin(), container.end(),
        [&uid](VT &p_variant) -> bool
        {
            return std::visit(
                [&](auto &var_val)
                {
                    // return uid == (std::get<std::decay_t<decltype(v)>>(p_variant)).get_uid();
                    return uid == std::decay_t<decltype(var_val)>(var_val).get_uid();
                },
                p_variant);
        });
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
    add_population(Network::AllPopulationVariants(population));
}


template <typename PopulationType>
void Network::add_population(typename std::decay<PopulationType>::type &population)
{
    SPDLOG_DEBUG("Add population");
    add_population(Network::AllPopulationVariants(population));
}


template <typename PopulationType>
PopulationType &Network::get_population(const knp::core::UID &population_uid)
{
    static_assert(
        boost::mp11::mp_contains<AllPopulations, PopulationType>(),
        "This population type doesn't supported by the Network class! Add type to the population types list.");

    SPDLOG_DEBUG("Get population {}", std::string(population_uid));
    auto pop_iterator = find_elem<PopulationType, core::AllPopulationsVariant>(population_uid, populations_);
    if (pop_iterator != populations_.end()) return std::get<PopulationType>(*pop_iterator);
    throw std::runtime_error("Can't find population!");
}


template <typename PopulationType>
const PopulationType &Network::get_population(const knp::core::UID &population_uid) const
{
    return const_cast<Network *>(this)->get_population<PopulationType>(population_uid);
}


void Network::remove_population(const core::UID &population_uid)
{
    SPDLOG_DEBUG("Remove population with uid {}", std::string(population_uid));
    auto result = find_variant<AllPopulations, core::AllPopulationsVariant>(population_uid, populations_);

    if (result == populations_.end()) throw std::runtime_error("Can't find population!");
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
    SPDLOG_DEBUG("Add projection {}", std::string(projection.get_uid()));
    add_projection(Network::AllProjectionVariants(projection));
}


template <typename ProjectionType>
void Network::add_projection(typename std::decay<ProjectionType>::type &projection)
{
    SPDLOG_DEBUG("Add projection {}", std::string(projection.get_uid()));
    add_projection(core::AllProjectionsVariant(projection));
}


template <typename ProjectionType>
ProjectionType &Network::get_projection(const knp::core::UID &projection_uid)
{
    static_assert(
        boost::mp11::mp_contains<core::AllProjections, ProjectionType>(),
        "This projection type doesn't supported by the Network class! Add type to the projection types list.");

    SPDLOG_DEBUG("Get projection {}", std::string(projection_uid));

    auto proj_iterator = find_elem<ProjectionType, core::AllProjectionsVariant>(projection_uid, projections_);
    if (proj_iterator != projections_.end()) return std::get<ProjectionType>(*proj_iterator);
    throw std::runtime_error("Can't find projection!");
}


template <typename ProjectionType>
const ProjectionType &Network::get_projection(const knp::core::UID &projection_uid) const
{
    return const_cast<Network *>(this)->get_projection<ProjectionType>(projection_uid);
}


void Network::remove_projection(const core::UID &projection_uid)
{
    SPDLOG_DEBUG("Remove projection with uid {}", std::string(projection_uid));
    auto result = find_variant<core::AllProjections, core::AllProjectionsVariant>(projection_uid, projections_);

    if (result == projections_.end()) throw std::runtime_error("Can't find projection!");
    projections_.erase(result);
}

#define INSTANCE_POPULATION_FUNCTIONS(n, template_for_instance, neuron_type)                                          \
    template void Network::add_population<knp::core::Population<neuron_type>>(knp::core::Population<neuron_type> &&); \
    template void Network::add_population<knp::core::Population<neuron_type>>(knp::core::Population<neuron_type> &);  \
    template knp::core::Population<neuron_type> &Network::get_population<knp::core::Population<neuron_type>>(         \
        const knp::core::UID &);                                                                                      \
    template const knp::core::Population<neuron_type> &Network::get_population<knp::core::Population<neuron_type>>(   \
        const knp::core::UID &) const;

#define INSTANCE_PROJECTION_FUNCTIONS(n, template_for_instance, synapse_type)                                          \
    template void Network::add_projection<knp::core::Projection<synapse_type>>(                                        \
        knp::core::Projection<synapse_type> &&);                                                                       \
    template void Network::add_projection<knp::core::Projection<synapse_type>>(knp::core::Projection<synapse_type> &); \
    template knp::core::Projection<synapse_type> &Network::get_projection<knp::core::Projection<synapse_type>>(        \
        const knp::core::UID &);                                                                                       \
    template const knp::core::Projection<synapse_type> &Network::get_projection<knp::core::Projection<synapse_type>>(  \
        const knp::core::UID &) const;

}  // namespace knp::framework
