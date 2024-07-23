/**
 * @file messaging.h
 * @brief Common header for messaging.
 * @author Artiom N.
 * @date 02.03.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/core/messaging/message_header.h>
#include <knp/core/messaging/spike_message.h>
#include <knp/core/messaging/synaptic_impact_message.h>

#include <boost/mp11.hpp>


/**
 * @brief Messaging namespace.
 */
namespace knp::core::messaging
{

/**
 * @brief List of all message types.
 */
#define ALL_MESSAGES SpikeMessage, SynapticImpactMessage

/**
 * @brief List of `boost::mp11` types. You can use the list to manage your message types.
 */
using AllMessages = boost::mp11::mp_list<ALL_MESSAGES>;

}  // namespace knp::core::messaging
