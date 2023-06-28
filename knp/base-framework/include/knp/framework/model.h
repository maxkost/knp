/**
 * @file model.h
 * @brief Model interface.
 * @author Artiom N.
 * @date 30.03.2023
 */

#pragma once

#include <knp/core/core.h>
#include <knp/core/impexp.h>
#include <knp/framework/backend_loader.h>
#include <knp/framework/io/input_channel.h>
#include <knp/framework/io/output_channel.h>
#include <knp/framework/network.h>

#include <memory>
#include <unordered_map>
#include <utility>


/**
 * @brief Framework namespace.
 */
namespace knp::framework
{

/**
 * @brief The Model class is a definition of a model that contains a network, input and output channels, a monitor and a
 * backend.
 */
class Model
{
public:
    /**
     * @brief Model constructor.
     * @param network network to use in the model.
     */
    explicit Model(knp::framework::Network &&network) : network_(std::move(network)) {}

public:
    /**
     * @brief Get model UID.
     * @return model UID.
     */
    [[nodiscard]] const knp::core::UID &get_uid() const { return base_.uid_; }
    /**
     * @brief Get tags used by the model.
     * @return model tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * @brief Get network associated with the model.
     * @return network.
     */
    [[nodiscard]] knp::framework::Network &get_network() { return network_; }
    /**
     * @brief Get network associated with the model.
     * @note Constant method.
     * @return network.
     */
    [[nodiscard]] const knp::framework::Network &get_network() const { return network_; }

    /**
     * @brief Add an input channel to the network.
     * @param channel input channel to be added.
     * @return channel UID.
     */
    core::UID add_input_channel(knp::framework::input::InputChannel &&channel)
    {
        core::UID uid = channel.get_uid();
        in_channels_.insert(std::make_pair(channel.get_uid(), std::move(channel)));
        return uid;
    }

    /**
     * @brief Add an output channel to the network.
     * @param channel_ptr
     * @return
     */
    core::UID add_output_channel(std::unique_ptr<knp::framework::output::OutputChannelBase> &&channel_ptr)
    {
        core::UID uid = channel_ptr->get_uid();
        out_channels_.insert(std::make_pair(channel_ptr->get_uid(), std::move(channel_ptr)));
        return uid;
    }

    /**
     * @brief Get output channel reference. Cast it to the type you need.
     * @param channel_uid channel UID.
     * @return base output channel class reference. It should be cast to the right type before extracting data from it.
     * @throw std::runtime_error if there is no channel with a given UID.
     */
    framework::output::OutputChannelBase &get_output_channel(const core::UID &channel_uid)
    {
        auto result = out_channels_.find(channel_uid);
        if (result == out_channels_.end() || !result->second) throw std::runtime_error("Wrong channel UID");
        return *(result->second);
    }

    /**
     * @brief Get an input channel reference by its UID.
     * @param channel_uid channel UID.
     * @return reference to a channel.
     * @throw std::runtime_error if no channel with that UID exists.
     */
    framework::input::InputChannel &get_input_channel(const core::UID &channel_uid)
    {
        auto result = in_channels_.find(channel_uid);
        if (result == in_channels_.end()) throw std::runtime_error("Wrong channel UID");
        return result->second;
    }

private:
    knp::core::BaseData base_;
    knp::framework::Network network_;
    std::unordered_map<core::UID, knp::framework::input::InputChannel, core::uid_hash> in_channels_;
    std::unordered_map<core::UID, std::unique_ptr<knp::framework::output::OutputChannelBase>, core::uid_hash>
        out_channels_;
};

}  // namespace knp::framework
