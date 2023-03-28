/**
 * @file network.cpp
 * @brief Network class implementation.
 * @author Artiom N.
 * @date 22.03.2023
 */

#include <knp/framework/network.h>

#include <spdlog/spdlog.h>


namespace knp::framework
{

void Network::add_population(Network::AllPopulationVariants &&population) {}


void Network::add_projection(Network::AllProjectionVariants &&projection) {}


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


Network::AllPopulationVariants &Network::get_population(const knp::core::UID &population_uid) {}


Network::AllProjectionVariants &Network::get_projection(const knp::core::UID &projection_uid) {}


const Network::AllPopulationVariants &Network::get_population(const knp::core::UID &population_uid) const {}


const Network::AllProjectionVariants &Network::get_projection(const knp::core::UID &projection_uid) const {}

}  // namespace knp::framework
