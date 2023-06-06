/**
 * @brief Output converter header
 * @author Vartenkov Andrey
 * @date 05.04.2023
 */

#pragma once
#include <functional>
#include <vector>

#include "knp/core/messaging/spike_message.h"


/**
 * @brief Output channel namespace.
 */
namespace knp::framework::output
{
/**
 * @brief Function used to convert spike messages into output data.
 * @tparam ResultType output data type.
 * @param messages list of spike messages.
 */
template <class ResultType>
using OutputConverter = std::function<ResultType(const std::vector<core::messaging::SpikeMessage> &messages)>;
}  // namespace knp::framework::output
