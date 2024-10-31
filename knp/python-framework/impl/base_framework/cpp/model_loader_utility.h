/**
 * @file model_loader_utility.h
 * @brief Auxiliary functions for ModelLoader class bindings.
 * @kaspersky_support Vartenkov A.
 * @date 30.10.2024
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

#include <knp/framework/model_loader.h>

#include <memory>
#include <utility>


knp::framework::ModelLoader make_model_loader(
    std::shared_ptr<knp::core::Backend> backend, knp::framework::ModelLoader::InputChannelMap i_map)
{
    return knp::framework::ModelLoader{backend, std::move(i_map)};
}


auto &get_model_loader_output_channel(knp::framework::ModelLoader &model_loader, const knp::core::UID &uid)
{
    return model_loader.get_output_channel(uid);
}


auto &get_model_loader_input_channel(knp::framework::ModelLoader &model_loader, const knp::core::UID &uid)
{
    return model_loader.get_input_channel(uid);
}
