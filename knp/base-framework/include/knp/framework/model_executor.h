/**
 * @file model_executor.h
 * @brief Model executor interface.
 * @author Artiom N.
 * @date 21.04.2023
 */

#pragma once

#include <knp/framework/backend_loader.h>
#include <knp/framework/model.h>
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
     * @brief Type of the input channel map.
     */
    using InputChannelMap = std::unordered_map<core::UID, input::DataGenerator, core::uid_hash>;

public:
    /**
     * @brief ModelExecutor constructor.
     * @param model model to run.
     * @param backend_path filepath to backend on which you want to run the model.
     * @param i_map input channel map.
     */
    ModelExecutor(knp::framework::Model &model, const std::filesystem::path &backend_path, InputChannelMap i_map)
        : backend_loader_(), model_(model), i_map_(std::move(i_map))
    {
        backend_ = backend_loader_.load(backend_path);
        init();
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
     * @brief Get reference to output channel.
     * @param channel_uid channel UID.
     * @return reference to output channel.
     * @throw std::runtime_error if there is no channel with a given UID.
     */
    output::OutputChannel &get_output_channel(const core::UID &channel_uid);
    /**
     * @brief Get reference to output channel.
     * @note Constant method.
     * @param channel_uid channel UID.
     * @return reference to output channel.
     * @throw std::runtime_error if there is no channel with a given UID.
     */
    const output::OutputChannel &get_output_channel(const core::UID &channel_uid) const;

    /**
     * @brief Get reference to input channel by its UID.
     * @param channel_uid channel UID.
     * @return reference to input channel.
     * @throw std::runtime_error if no channel with the given UID exists.
     */
    input::InputChannel &get_input_channel(const core::UID &channel_uid);
    /**
     * @brief Get reference to input channel by its UID.
     * @note Constant method.
     * @param channel_uid channel UID.
     * @return reference to input channel.
     * @throw std::runtime_error if no channel with the given UID exists.
     */
    const input::InputChannel &get_input_channel(const core::UID &channel_uid) const;

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
            backend_->message_bus_.create_endpoint(), std::move(message_processor), core::UID{true}));

        std::visit([&senders](auto &entity) { entity.subscribe(senders); }, observers_.back());
    }

    /**
     * @brief Lock synapse weights.
     */
    void lock_weights() { backend_->stop_learning(); }

    /**
     * @brief Unlock synapse weights.
     */
    void unlock_weights() { backend_->start_learning(); }

    core::Backend *get_backend() { return backend_.get(); }

protected:
    /**
     * @brief Write model to backend.
     */
    void init();

private:
    template <typename GenType>
    void init_channels(
        const std::unordered_multimap<core::UID, core::UID, core::uid_hash> &channels, GenType channel_gen);

    void gen_input_channel(const core::UID &, const std::vector<core::UID> &);
    void gen_output_channel(const core::UID &, const std::vector<core::UID> &);

private:
    knp::core::BaseData base_;
    knp::framework::BackendLoader backend_loader_;
    std::shared_ptr<core::Backend> backend_;
    knp::framework::Model &model_;
    InputChannelMap i_map_;
    std::vector<knp::framework::input::InputChannel> in_channels_;
    std::vector<knp::framework::output::OutputChannel> out_channels_;
    std::vector<monitoring::AnyObserverVariant> observers_;
};
}  // namespace knp::framework
