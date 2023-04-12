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
public:
private:
    knp::core::BaseData base_;
    std::shared_ptr<knp::core::Backend> backend_;
    knp::framework::Network network_;
};

}  // namespace knp::framework
