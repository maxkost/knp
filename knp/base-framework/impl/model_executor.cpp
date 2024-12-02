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
            // Running handlers
            for (auto &handler : message_handlers_)
            {
                handler.update(get_backend()->get_step());
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


void ModelExecutor::SpikeMessageHandler::update(size_t step)
{
    endpoint_.receive_all_messages();
    auto incoming_messages = endpoint_.unload_messages<MessageIn>(base_.uid_);
    knp::core::messaging::SpikeMessage outgoing_message = {
        {base_.uid_, step}, message_handler_function_(incoming_messages)};
    if (!(outgoing_message.neuron_indexes_.empty()))
    {
        endpoint_.send_message(outgoing_message);
    }
}


void ModelExecutor::add_spike_message_handler(
    typename SpikeMessageHandler::FunctionType &&message_handler_function, const std::vector<core::UID> &senders,
    const std::vector<core::UID> &receivers, const knp::core::UID &uid)
{
    knp::core::MessageEndpoint endpoint = get_backend()->get_message_bus().create_endpoint();
    message_handlers_.emplace_back(SpikeMessageHandler{std::move(message_handler_function), std::move(endpoint), uid});
    message_handlers_.back().subscribe(senders);
    for (const knp::core::UID &rec_uid : receivers)
    {
        get_backend()->subscribe<knp::core::messaging::SpikeMessage>(rec_uid, {uid});
    }
}

}  // namespace knp::framework
