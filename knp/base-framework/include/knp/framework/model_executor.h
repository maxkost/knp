/**
 * @file model_executor.h
 * @brief Model executor interface.
 * @author Artiom N.
 * @date 21.04.2023
 */

#pragma once

#include <knp/framework/backend_loader.h>
#include <knp/framework/model.h>

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

    /**
     * @brief Write model to backend.
     */
    void init();

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
    // cppcheck-suppress unusedStructMember
    std::vector<knp::framework::input::InputChannel> in_channels_;

    // cppcheck-suppress unusedStructMember
    std::vector<knp::framework::output::OutputChannel> out_channels_;
};

}  // namespace knp::framework
