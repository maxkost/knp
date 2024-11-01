/**
 * @file network_utility.h
 * @brief Auxiliary functions for Network bindings.
 * @kaspersky_support Vartenkov A.
 * @date 18.06.2024
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

#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/framework/network.h>

#include <utility>


void network_add_projection_variant(knp::framework::Network &self, knp::core::AllProjectionsVariant proj)
{
    self.add_projection(std::move(proj));
}


void network_add_population_variant(knp::framework::Network &self, knp::core::AllPopulationsVariant pop)
{
    self.add_population(std::move(pop));
}


knp::core::AllPopulationsVariant network_get_population_variant(
    knp::framework::Network &self, const knp::core::UID &population_uid)
{
    return self.get_population(population_uid);
}


knp::core::AllProjectionsVariant network_get_projection_variant(
    knp::framework::Network &self, const knp::core::UID &projection_uid)
{
    return self.get_projection(projection_uid);
}


auto network_begin_populations(knp::framework::Network &self)
{
    return self.begin_populations();
}


auto network_end_populations(knp::framework::Network &self)
{
    return self.end_populations();
}


auto network_begin_projections(knp::framework::Network &self)
{
    return self.begin_projections();
}


auto network_end_projections(knp::framework::Network &self)
{
    return self.end_projections();
}
