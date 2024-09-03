/**
 * @file message_endpoint.h
 * @brief Python bindings header for message endpoint.
 * @author Artiom N.
 * @date 26.02.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/core/message_endpoint.h>

#include <boost/python.hpp>


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(receive_all_messages_overloads, receive_all_messages, 0, 1);
