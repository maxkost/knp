/**
 * @file messaging.h
 * @brief Messaging common header.
 * @author Artiom N.
 * @date 02.03.2023
 */

#pragma once

#include <knp/core/messaging/spike_message.h>
#include <knp/core/messaging/synaptic_impact_message.h>

#include <boost/mp11.hpp>


namespace knp::core::messaging
{

#define ALL_MESSAGES SpikeMessage, SynapticImpactMessage

using AllMessages = boost::mp11::mp_list<ALL_MESSAGES>;

}  // namespace knp::core::messaging
