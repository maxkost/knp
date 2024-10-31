/**
 * @file model_utility.h
 * @brief Auxiliary functions for Model class bindings.
 * @kaspersky_support Vartenkov A.
 * @date 05.06.2024
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
#include <knp/framework/model.h>

#include <memory>
#include <utility>


std::shared_ptr<knp::framework::Model> model_constructor(knp::framework::Network &network)
{
    knp::framework::Model model{std::move(network)};
    return std::make_shared<knp::framework::Model>(std::move(model));
}


// A very bad and wrong way to access network from model
knp::framework::Network get_network_from_model(knp::framework::Model &self)
{
    return self.get_network();
}


void set_network_to_model(knp::framework::Model &self, knp::framework::Network &network)
{
    self.get_network() = std::move(network);
}


auto get_model_input_channels(knp::framework::Model &self)
{
    return self.get_input_channels();
}


auto get_model_output_channels(knp::framework::Model &self)
{
    return self.get_output_channels();
}
