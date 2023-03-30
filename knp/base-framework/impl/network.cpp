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


template <typename T, typename VT, auto Container>
typename decltype(Container)::iterator &Network::find_elem(const knp::core::UID &uid)
{
    constexpr auto type_n = boost::mp11::mp_find<VT, T>();
    auto result = std::find_if(
        Container.begin(), Container.end(),
        [&uid](VT &p_variant) -> bool
        {
            if (p_variant.index() != type_n) return false;
            return uid == (std::get<type_n>(p_variant)).get_uid();
        });
    return result;
}


void Network::add_population(Network::AllPopulationVariants &&population)
{
    populations_.emplace_back(population);
}


template <typename PopulationType>
void Network::add_population(PopulationType &&population)
{
    add_population(Network::AllPopulationVariants(population));
}


template <typename PopulationType>
PopulationType &Network::get_population(const knp::core::UID &population_uid)
{
    static_assert(
        boost::mp11::mp_contains<AllPopulations, PopulationType>(),
        "This population type doesn't supported by the Network class! Add type to the population types list.");

    SPDLOG_DEBUG("Get population {}", population_uid);
    auto r = find_elem<PopulationType, AllPopulationVariants, this->populations_>(population_uid);
    if (r != populations_.end()) return std::get<boost::mp11::mp_find<AllPopulations, PopulationType>>(*r);
    throw std::runtime_error("Can't find population!");
}


template <typename PopulationType>
const PopulationType &Network::get_population(const knp::core::UID &population_uid) const
{
    return const_cast<Network *>(this)->get_population<PopulationType>(population_uid);
}


template <typename PopulationType>
void Network::remove_population(const knp::core::UID &population_uid)
{
    SPDLOG_DEBUG("Remove population {}", population_uid);
    auto r = find_elem<PopulationType, AllPopulationVariants, this->populations_>(population_uid);
    if (r != populations_.end())
    {
        populations_.erase(r);
    }
    else
        throw std::runtime_error("Can't find population!");
}


void Network::remove_population(const core::UID &population_uid)
{
    // if (r != projections_.end()) return std::get<boost::mp11::mp_find<AllProjections, ProjectionType>>(*r);
}


void Network::add_projection(Network::AllProjectionVariants &&projection)
{
    SPDLOG_DEBUG("Add projection variant");
    projections_.emplace_back(projection);
}


template <typename ProjectionType>
void Network::add_projection(ProjectionType &&projection)
{
    SPDLOG_DEBUG("Add projection {}", projection.uid);
    add_projection(Network::AllPopulationVariants(projection));
}


template <typename ProjectionType>
ProjectionType &Network::get_projection(const knp::core::UID &projection_uid)
{
    static_assert(
        boost::mp11::mp_contains<AllProjections, ProjectionType>(),
        "This projection type doesn't supported by the Network class! Add type to the projection types list.");

    SPDLOG_DEBUG("Get projection {}", projection_uid);

    auto r = find_elem<ProjectionType, AllProjectionVariants, this->projections_>(projection_uid);
    if (r != projections_.end()) return std::get<boost::mp11::mp_find<AllProjections, ProjectionType>>(*r);
    throw std::runtime_error("Can't find projection!");
}


template <typename ProjectionType>
const ProjectionType &Network::get_projection(const knp::core::UID &projection_uid) const
{
    return const_cast<Network *>(this)->get_projection<ProjectionType>(projection_uid);
}


template <typename ProjectionType>
void Network::remove_projection(const knp::core::UID &projection_uid)
{
    SPDLOG_DEBUG("Remove projection {}", population_uid);
    auto r = find_elem<ProjectionType, AllPopulationVariants, this->populations_>(projection_uid);
    if (r != projections_.end())
    {
        projections_.erase(r);
    }
    else
        throw std::runtime_error("Can't find projection!");
}


void Network::remove_projection(const core::UID &projection_uid) {}


#define INSTANCE_POPULATION_FUNCTIONS(n, template_for_instance, population_type)                                     \
    template <>                                                                                                      \
    void Network::add_population<knp::core::Population<population_type>>(knp::core::Population<population_type> &&); \
    template <>                                                                                                      \
    knp::core::Population<population_type> &Network::get_population<knp::core::Population<population_type>>(         \
        const knp::core::UID &);                                                                                     \
    template <>                                                                                                      \
    const knp::core::Population<population_type> &Network::get_population<knp::core::Population<population_type>>(   \
        const knp::core::UID &) const;                                                                               \
    template <>                                                                                                      \
    void Network::remove_population<knp::core::Population<population_type>>(const knp::core::UID &);

#define INSTANCE_PROJECTION_FUNCTIONS(n, template_for_instance, projection_type)                                     \
    template <>                                                                                                      \
    void Network::add_projection<knp::core::Projection<projection_type>>(knp::core::Projection<projection_type> &&); \
    template <>                                                                                                      \
    knp::core::Projection<projection_type> &Network::get_projection<knp::core::Projection<projection_type>>(         \
        const knp::core::UID &);                                                                                     \
    template <>                                                                                                      \
    const knp::core::Projection<projection_type> &Network::get_projection<knp::core::Projection<projection_type>>(   \
        const knp::core::UID &) const;                                                                               \
    template <>                                                                                                      \
    void Network::remove_projection<knp::core::Projection<projection_type>>(const knp::core::UID &);

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_POPULATION_FUNCTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_NEURONS))
// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_PROJECTION_FUNCTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_SYNAPSES))

}  // namespace knp::framework
