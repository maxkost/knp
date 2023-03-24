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


Network::AllPopulationVariants &Network::get_population(const knp::core::UID &population_uid) {}


Network::AllProjectionVariants &Network::get_projection(const knp::core::UID &projection_uid) {}


const Network::AllPopulationVariants &Network::get_population(const knp::core::UID &population_uid) const {}


const Network::AllProjectionVariants &Network::get_projection(const knp::core::UID &projection_uid) const {}

}  // namespace knp::framework
