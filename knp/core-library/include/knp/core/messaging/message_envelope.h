/**
 * @file message_envelope.h
 * @brief Message envelope routines.
 * @author Artiom N.
 * @date 13.04.2023
 */

#pragma once

#include <knp/core/uid.h>

#include <iostream>
#include <variant>
#include <vector>

#include "messaging.h"


namespace knp::core::messaging
{

std::vector<uint8_t> pack_to_envelope(boost::mp11::mp_rename<AllMessages, std::variant> message);
boost::mp11::mp_rename<AllMessages, std::variant> extract_from_envelope(const void *buffer);
boost::mp11::mp_rename<AllMessages, std::variant> extract_from_envelope(std::vector<uint8_t> &buffer);

}  // namespace knp::core::messaging
