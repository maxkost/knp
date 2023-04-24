/**
 * @file messaging.h
 * @brief Messaging common header.
 * @author Artiom N.
 * @date 02.03.2023
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
#define ALL_MESSAGES knp::core::messaging::SpikeMessage, knp::core::messaging::SynapticImpactMessage

/**
 * @brief Boost::mp11 types list. Used for type manipulations.
 */
using AllMessages = boost::mp11::mp_list<ALL_MESSAGES>;

}  // namespace knp::core::messaging
