/**
 * @file describe_network.h
 * @brief Print network statistics to console.
 * @kaspersky_support A. Vartenkov
 * @date 04.04.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once
#include <knp/framework/network.h>


template <class Projection>
void describe_projection(const Projection &proj)
{
    std::cout << "Projection " << proj.get_uid() << std::endl;
    std::cout << "\tsize: " << proj.size() << " synapses of type " << typeid(std::get<0>(*proj.begin())).name()
              << std::endl;
    double avg_delay = 0.0, avg_weight = 0.0;
    for (const auto &synapse : proj)
    {
        avg_delay += std::get<0>(synapse).delay_;
        avg_weight += std::get<0>(synapse).weight_;
    }
    avg_delay /= proj.size();
    avg_weight /= proj.size();
    std::cout << "\tavg. weight: " << avg_weight << ", avg. delay: " << avg_delay << std::endl;
}


template <class Population>
void describe_population(const Population &pop)
{
    std::cout << "Population " << pop.get_uid() << std::endl;
    std::cout << "\tsize: " << pop.size() << " neurons of type " << typeid(*pop.begin()).name() << std::endl;
}


void describe_network(const knp::framework::Network &network)
{
    std::cout << "Network " << network.get_uid() << std::endl;
    std::cout << "Populations: " << std::endl;
    for (const auto &pop : network.get_populations())
        std::visit([](const auto &population) { describe_population(population); }, pop);

    for (const auto &proj : network.get_projections())
        std::visit([](const auto &projection) { describe_projection(projection); }, proj);
    std::cout << std::endl;
}
