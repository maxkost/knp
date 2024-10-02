/**
 * @file model.h
 * @brief Model interface.
 * @author Artiom N.
 * @date 30.03.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/core/core.h>
#include <knp/core/impexp.h>
#include <knp/framework/backend_loader.h>
#include <knp/framework/io/input_channel.h>
#include <knp/framework/io/output_channel.h>
#include <knp/framework/monitoring/observer.h>
#include <knp/framework/network.h>

#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>


/**
 * @brief Framework namespace.
 */
namespace knp::framework
{

/**
 * @brief The Model class is a definition of a model that contains a network, input and output channels, a monitor and a
 * backend.
 */
class KNP_DECLSPEC Model
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
     * @param channel_uid UID of the input channel.
     * @param projection_uid UID of the projection which will be connected to the channel.
     */
    void add_input_channel(const core::UID &channel_uid, const core::UID &projection_uid);
    /**
     * @brief Add an output channel to the network.
     * @param channel_uid UID of the channel object.
     * @param population_uid UID of the population which will be connected to the channel.
     */
    void add_output_channel(const core::UID &channel_uid, const core::UID &population_uid);

    /**
     * @brief Return all input channels.
     * @return map of input channels to projections.
     */
    const std::unordered_multimap<core::UID, core::UID, core::uid_hash> &get_input_channels() const;
    /**
     * @brief Return all output channels.
     * @return map of output channels to populations.
     */
    const std::unordered_multimap<core::UID, core::UID, core::uid_hash> &get_output_channels() const;

private:
    knp::core::BaseData base_;
    knp::framework::Network network_;
    // cppcheck-suppress unusedStructMember
    std::unordered_multimap<core::UID, core::UID, core::uid_hash> in_channels_;
    // cppcheck-suppress unusedStructMember
    std::unordered_multimap<core::UID, core::UID, core::uid_hash> out_channels_;
};

}  // namespace knp::framework
