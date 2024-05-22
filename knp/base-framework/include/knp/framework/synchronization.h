/**
 * @file synchronization.h
 * @brief Functions to read network back from .
 * @author Artiom N.
 * @date 22.03.2023
 */

#pragma once

#include <knp/core/backend.h>
#include <knp/framework/network.h>


namespace knp::framework::synchronization
{
/**
 * @brief Copies network data from backend.
 * @param backend Backend containing a network.
 * @return Network as it currently is on a backend.
 */
Network get_network_copy(const knp::core::Backend &backend);

}  // namespace knp::framework::synchronization
