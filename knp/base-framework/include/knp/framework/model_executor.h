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
    using InputChannelGenerator =
        std::function<std::unique_ptr<input::InputChannel>(const core::UID &, core::MessageEndpoint &&)>;
    using OutputChannelGenerator =
        std::function<std::unique_ptr<output::OutputChannelBase>(const core::UID &, core::MessageEndpoint &&)>;

public:
    /**
     * @brief ModelExecutor constructor.
     * @param model model to run.
     * @param backend_path filepath to backend on which you want to run the model.
     * @param i_gen input channels generator.
     * @param out_gen output channels generator.
     */
    ModelExecutor(
        knp::framework::Model &model, const std::filesystem::path backend_path, InputChannelGenerator i_gen,
        OutputChannelGenerator out_gen)
        : backend_loader_(), model_(model), i_gen_(i_gen), out_gen_(out_gen)
    {
        backend_ = backend_loader_.load(backend_path);
    }

public:
    /**
     * @brief Start model execution.
     */
    void start();

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
     * @brief Get output channel reference. Cast it to the type you need.
     * @param channel_uid channel UID.
     * @return base output channel class reference. It should be cast to the right type before extracting data
     from it.
     * @throw std::runtime_error if there is no channel with a given UID.
     */
    output::OutputChannelBase *get_output_channel(const core::UID &channel_uid);
    /**
     * @brief Get an input channel reference by its UID.
     * @param channel_uid channel UID.
     * @return reference to a channel.
     * @throw std::runtime_error if no channel with that UID exists.
     */
    input::InputChannel *get_input_channel(const core::UID &channel_uid);

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
    InputChannelGenerator i_gen_;
    OutputChannelGenerator out_gen_;
    // cppcheck-suppress unusedStructMember
    std::vector<std::unique_ptr<knp::framework::input::InputChannel>> in_channels_;
    // cppcheck-suppress unusedStructMember
    std::vector<std::unique_ptr<knp::framework::output::OutputChannelBase>> out_channels_;
};

}  // namespace knp::framework
