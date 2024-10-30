/**
 * @file model_executor.cpp
 * @brief Model executor class implementation.
 * @kaspersky_support Artiom N.
 * @date 21.04.2023
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

#include <knp/framework/model_executor.h>

#include <spdlog/spdlog.h>


namespace knp::framework
{

void ModelExecutor::start()
{
    start([](knp::core::Step) { return true; });
}


void ModelExecutor::start(core::Backend::RunPredicate run_predicate)
{
    SPDLOG_INFO("Starting model execution...");

    get_backend()->start(
        [this, run_predicate](knp::core::Step step)
        {
            // Sending inputs from the channels.
            for (auto &i_ch : loader_.get_inputs())
            {
                i_ch.send(step);
            }
            // Run user predicate.
            return run_predicate(step);
        },
        [this](knp::core::Step)
        {
            // Loading spikes into output channels.
            for (auto &o_ch : loader_.get_outputs())
            {
                o_ch.update();
            }
            // Run monitoring observers.
            for (auto &observer : observers_)
            {
                std::visit([](auto &entity) { entity.update(); }, observer);
            }

            return true;
        });
    SPDLOG_INFO("Model execution stopped.");
}


void ModelExecutor::stop()
{
    get_backend()->stop();
}

}  // namespace knp::framework
