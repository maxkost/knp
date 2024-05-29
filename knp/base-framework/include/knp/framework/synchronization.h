/**
 * @file synchronization.h
 * @brief Functions to read network back from .
 * @author Artiom N.
 * @date 22.03.2023
 */

#pragma once

#include <knp/core/backend.h>
#include <knp/framework/network.h>


/**
 * @brief Synchronization namespace.
 */
namespace knp::framework::synchronization
{
/**
 * @brief Copy network data from backend.
 * @param backend backend containing a network.
 * @return Network in its current state on backend.
 */
Network get_network_copy(const knp::core::Backend &backend);

}  // namespace knp::framework::synchronization
