/**
 * @file message_endpoint.h
 * @brief Message endpoint Python bindings header.
 * @author Artiom N.
 * @date 26.02.2024
 */

#pragma once

#include <knp/core/message_endpoint.h>

#include <boost/python.hpp>


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(
    receive_all_messages_overloads, core::MessageEndpoint::receive_all_messages, 0, 1);
