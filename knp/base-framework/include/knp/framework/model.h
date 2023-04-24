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
#include <knp/framework/network.h>

#include <memory>
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
    explicit Model(knp::framework::Network &&network) : network_(std::move(network)) {}

public:
    /**
     * @brief Get the model UID.
     * @return UID.
     */
    [[nodiscard]] const knp::core::UID &get_uid() const { return base_.uid_; }
    /**
     * @brief Get tags used by the model.
     * @return tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * @brief Get model's network.
     * @return network.
     */
    [[nodiscard]] knp::framework::Network &get_network() { return network_; }
    [[nodiscard]] const knp::framework::Network &get_network() const { return network_; }

private:
    knp::core::BaseData base_;
    knp::framework::Network network_;
};

}  // namespace knp::framework
