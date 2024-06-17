/**
 * @file model_executor.h
 * @brief Model executor interface.
 * @author Artiom N.
 * @date 21.04.2023
 */

#pragma once

#include <knp/core/impexp.h>
#include <knp/framework/backend_loader.h>
#include <knp/framework/io/input_converter.h>
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
class ModelExecutor
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
     * @brief Unlock synapse weights.
     */
    void start_learning() { get_backend()->start_learning(); }

    /**
     * @brief Lock synapse weights.
     */
    void stop_learning() { get_backend()->stop_learning(); }

    /**
     * @brief Get pointer to backend object.
     * @return shared pointer to Backend object.
     */
    std::shared_ptr<core::Backend> get_backend() { return loader_.get_backend(); }

    /**
     * @brief Get model loader object.
     * @return reference to ModelLoader object.
     */
    auto &get_loader() { return loader_; }

private:
    knp::core::BaseData base_;
    ModelLoader loader_;

    std::vector<monitoring::AnyObserverVariant> observers_;
};
}  // namespace knp::framework
