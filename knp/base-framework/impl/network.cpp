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

void Network::add_population(Network::AllPopulationVariants &&population)
{
    populations_.emplace_back(population);
}


void Network::add_projection(Network::AllProjectionVariants &&projection)
{
    projections_.emplace_back(projection);
}


void Network::remove_projection(const core::UID &projection_uid) {}


void Network::remove_population(const core::UID &population_uid) {}


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


template <typename PopulationType>
PopulationType &Network::get_population(const knp::core::UID &population_uid)
{
    static_assert(
        boost::mp11::mp_contains<AllPopulations, PopulationType>(),
        "This population type doesn't supported by the Network class! Add type to the population types list.");

    std::find_if(
        populations_.begin(), populations_.end(),
        [&population_uid](AllPopulationVariants &p_variant) -> PopulationIterator {
            return population_uid ==
                   (std::get<boost::mp11::mp_find<AllPopulations, PopulationType>>(p_variant)).get_uid();
        });
}

template <typename PopulationType>
const PopulationType &Network::get_population(const knp::core::UID &population_uid) const
{
    return const_cast<Network *>(this)->get_population<PopulationType>(population_uid);
}


template <typename ProjectionType>
ProjectionType &Network::get_projection(const knp::core::UID &projection_uid)
{
    static_assert(
        boost::mp11::mp_contains<AllProjections, ProjectionType>(),
        "This projection type doesn't supported by the Network class! Add type to the projection types list.");

    std::find_if(
        projections_.begin(), projections_.end(),
        [&projection_uid](AllPopulationVariants &p_variant) -> ProjectionIterator {
            return projection_uid ==
                   (std::get<boost::mp11::mp_find<AllProjections, ProjectionType>>(p_variant)).get_uid();
        });
}


template <typename ProjectionType>
const ProjectionType &Network::get_projection(const knp::core::UID &projection_uid) const
{
    return const_cast<Network *>(this)->get_projection<ProjectionType>(projection_uid);
}


#define INSTANCE_POPULATION_FUNCTIONS(n, template_for_instance, population_type)                                   \
    template <>                                                                                                    \
    knp::core::Population<population_type> &Network::get_population<knp::core::Population<population_type>>(       \
        const knp::core::UID &);                                                                                   \
    template <>                                                                                                    \
    const knp::core::Population<population_type> &Network::get_population<knp::core::Population<population_type>>( \
        const knp::core::UID &) const;

#define INSTANCE_PROJECTION_FUNCTIONS(n, template_for_instance, projection_type)                                   \
    template <>                                                                                                    \
    knp::core::Projection<projection_type> &Network::get_projection<knp::core::Projection<projection_type>>(       \
        const knp::core::UID &);                                                                                   \
    template <>                                                                                                    \
    const knp::core::Projection<projection_type> &Network::get_projection<knp::core::Projection<projection_type>>( \
        const knp::core::UID &) const;

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_POPULATION_FUNCTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_NEURONS))
// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_PROJECTION_FUNCTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_SYNAPSES))

}  // namespace knp::framework
