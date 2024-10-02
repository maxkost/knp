/**
 * @file output_converter.h
 * @brief Header for output converter.
 * @author Vartenkov Andrey
 * @date 05.04.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/core/messaging/spike_message.h>

#include <functional>
#include <vector>


/**
 * @brief Output channel namespace.
 */
namespace knp::framework::io::output
{
/**
 * @brief Function used to convert spike messages into output data.
 * @tparam ResultType output data type.
 * @param messages list of spike messages.
 */
template <class ResultType>
using OutputConverter = std::function<ResultType(const std::vector<core::messaging::SpikeMessage> &messages)>;
}  // namespace knp::framework::io::output
