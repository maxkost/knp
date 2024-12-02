/**
 * @file model_executor.h
 * @brief Model executor interface.
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

#pragma once

#include <knp/core/impexp.h>
#include <knp/framework/backend_loader.h>
#include <knp/framework/io/input_converter.h>
#include <knp/framework/message_handlers.h>
#include <knp/framework/model.h>
#include <knp/framework/model_loader.h>
#include <knp/framework/monitoring/observer.h>

#include <filesystem>
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>


/**
 * @brief Framework namespace.
 */
namespace knp::framework
{
/**
 * @brief The ModelExecutor class is a definition of an executor that runs the uploaded model on the specified backend.
 */
class KNP_DECLSPEC ModelExecutor
{
public:
    /**
     * @brief ModelExecutor constructor.
     * @param model model to run.
     * @param backend pointer to backend on which you want to run the model.
     * @param i_map input channel map.
     */
    ModelExecutor(
        knp::framework::Model &model, std::shared_ptr<core::Backend> backend, ModelLoader::InputChannelMap i_map)
        : loader_(backend, i_map)
    {
        loader_.load(model);
    }

public:
    /**
     * @brief Start model execution.
     */
    void start();

    /**
     * @brief Start model execution.
     * @param run_predicate predicate that stops running if the `false` value is returned.
     */
    void start(core::Backend::RunPredicate run_predicate);

    /**
     * @brief Stop model execution.
     */
    void stop();

public:
    /**
     * @brief Add observer to executor.
     * @tparam Message type of messages to observe.
     * @param message_processor functor to process received messages.
     * @param senders list of observed entities.
     */
    template <class Message>
    void add_observer(monitoring::MessageProcessor<Message> &&message_processor, const std::vector<core::UID> &senders)
    {
        observers_.emplace_back(monitoring::MessageObserver<Message>(
            get_backend()->get_message_bus().create_endpoint(), std::move(message_processor), core::UID{true}));

        std::visit([&senders](auto &entity) { entity.subscribe(senders); }, observers_.back());
    }

    /**
     * @brief Function type for message handlers.
     */
    using SpikeHandlerFunction =
        std::function<knp::core::messaging::SpikeData(std::vector<knp::core::messaging::SpikeMessage> &)>;

    /**
     * @brief Add spike message handler to executor.
     * @param message_handler_function functor to process received messages.
     * @param senders list of entities sending messages to the handler.
     * @param receivers list of entities receiving messages from handler.
     * @param uid handler uid.
     */
    void add_spike_message_handler(
        SpikeHandlerFunction &&message_handler_function, const std::vector<core::UID> &senders,
        const std::vector<core::UID> &receivers, const knp::core::UID &uid = knp::core::UID{});

    /**
     * @brief Unlock synapse weights.
     */
    void start_learning() { get_backend()->start_learning(); }

    /**
     * @brief Lock synapse weights.
     */
    void stop_learning() { get_backend()->stop_learning(); }

    /**
     * @brief Get pointer to backend object.
     * @return shared pointer to `Backend` object.
     */
    std::shared_ptr<core::Backend> get_backend() { return loader_.get_backend(); }

    /**
     * @brief Get model loader object.
     * @return reference to `ModelLoader` object.
     */
    auto &get_loader() { return loader_; }

private:
    /**
     * @brief An object that receives and processes messages.
     */
    class SpikeMessageHandler
    {
    public:
        using MessageIn = knp::core::messaging::SpikeMessage;
        using MessageData = knp::core::messaging::SpikeData;
        using FunctionType = std::function<MessageData(std::vector<MessageIn> &)>;

        SpikeMessageHandler(
            FunctionType &&function, knp::core::MessageEndpoint &&endpoint, const knp::core::UID &uid = {})
            : message_handler_function_(std::move(function)), endpoint_(std::move(endpoint)), base_{uid}
        {
        }

        SpikeMessageHandler(SpikeMessageHandler &&other) noexcept = default;

        SpikeMessageHandler(const SpikeMessageHandler &) = delete;

        void subscribe(const std::vector<core::UID> &entities) { endpoint_.subscribe<MessageIn>(base_.uid_, entities); }

        void update(size_t step);

        [[nodiscard]] knp::core::UID get_uid() const { return base_.uid_; };

    private:
        FunctionType message_handler_function_;
        core::MessageEndpoint endpoint_;
        knp::core::BaseData base_;
    };

    knp::core::BaseData base_;
    ModelLoader loader_;

    std::vector<monitoring::AnyObserverVariant> observers_;
    std::vector<SpikeMessageHandler> message_handlers_;
};
}  // namespace knp::framework
