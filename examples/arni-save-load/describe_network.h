/**
 * @file describe_network.h
 * @brief Print network statistics to console.
 * @author A. Vartenkov
 * @date 04.04.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
