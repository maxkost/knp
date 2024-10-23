//
// Created by an_vartenkov on 18.06.24.
//

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
