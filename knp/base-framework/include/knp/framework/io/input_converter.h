/**
 * @brief Input converter header
 * @author Vartenkov Andrey
 * @date 25.04.2023
 */
#pragma once

#include <knp/core/message_endpoint.h>
#include <knp/core/messaging/spike_message.h>
#include <knp/core/uid.h>

#include <spdlog/spdlog.h>

#include <functional>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


/**
 * @brief Input channel namespace.
 */
namespace knp::framework::input
{
/**
 * @brief Functor used for converting input stream data to spikes.
 */
using DataConverter = std::function<core::messaging::SpikeData(std::istream &)>;

}  // namespace knp::framework::input
