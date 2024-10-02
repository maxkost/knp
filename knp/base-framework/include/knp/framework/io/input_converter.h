/**
 * @file input_converter.h
 * @brief Header for input converter.
 * @author Vartenkov Andrey
 * @date 25.04.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
#pragma once

#include <knp/core/impexp.h>
#include <knp/core/message_endpoint.h>
#include <knp/core/messaging/spike_message.h>
#include <knp/core/uid.h>

#include <functional>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


/**
 * @brief Input channel namespace.
 */
namespace knp::framework::io::input
{
/**
 * @brief Functor used for generating input spike messages.
 */
using DataGenerator = std::function<core::messaging::SpikeData(core::Step)>;

}  // namespace knp::framework::io::input
