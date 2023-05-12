/**
 * @brief Output converter header
 * @author Vartenkov Andrey
 * @date 05.04.2023
 */

#pragma once
#include <knp/core/messaging/spike_message.h>

#include <functional>
#include <set>
#include <vector>

/**
 * @brief Network output processing namespace.
 */
namespace knp::core::output
{
using MessageList = std::vector<messaging::SpikeMessage>;

/**
 * @brief a function to convert messages into data.
 * @tparam ResultType resulting data type.
 * @param MessageList list of spike messages.
 * @param TimeType step index of the oldest messages that are considered.
 * @note use 0 for "any message" or current step value for current step messages only.
 */
template <class ResultType>
using OutputConverter = std::function<ResultType(const MessageList &, size_t, messaging::TimeType)>;


/**
 * @brief Convert a set of spike messages to a vector, so that result[neuron] = number of times neuron spiked.
 * @param message_list a list of spike messages.
 * @param output_size output population size.
 * @param oldest_step if a message is older than this, it's skipped.
 *                    Zero means that all messages since last call are used for counting.
 * @return vector of number of emitted spikes per neuron.
 * @details this converter(out_size = 6) will convert messages {0, 2}, {2, 4}, {1, 2} to (1, 1, 3, 0, 1, 0}.
 */
std::vector<size_t> converter_count(
    const MessageList &message_list, size_t output_size, messaging::TimeType oldest_step = 0)
{
    std::vector<size_t> result(output_size, 0);
    for (auto &message : message_list)
    {
        if (message.header_.send_time_ < oldest_step) continue;

        for (auto index : message.neuron_indexes_)
        {
            if (index < output_size) ++result[index];
        }
    }
    return result;
}


/**
 * @brief Convert messages to bool vector where an element is true if a corresponding neuron sent at least one spike.
 * @param message_list spike messages.
 * @param out_size output population size.
 * @param oldest_step if a message is older than this, it's skipped.
 *                    Zero means that all messages since last call are used for counting.
 * @return a function that converts spike messages to a bool vector of "true if the corresponding neuron spiked".
 * @details this converter(out_size=6) will convert messages {0, 2}, {2, 4}, {1, 2} to boolean vector {111010}.
 */
std::vector<bool> converter_bitwise(
    const MessageList &message_list, size_t output_size, messaging::TimeType oldest_step = 0)
{
    std::vector<bool> result(output_size, false);
    for (auto &message : message_list)
    {
        if (message.header_.send_time_ < oldest_step) continue;

        for (auto index : message.neuron_indexes_)
        {
            if (index < output_size) result[index] = true;
        }
    }
    return result;
}


/**
 * @brief Get a set of all recently spiked neurons.
 * @param message_list spike messages.
 * @param output_size output population size, neuron indexes greater than output_size are ignored.
 * @param oldest_step if a message is older than this, it is skipped.
 *                    Zero means that all messages since last call are used.
 * @return set of all neurons that spiked at a period defined by message_list and oldest_step.
 */
std::set<messaging::SpikeIndexType> converter_to_set(
    const MessageList &message_list, size_t output_size, messaging::TimeType oldest_step = 0)
{
    std::set<messaging::SpikeIndexType> result;
    for (auto &message : message_list)
    {
        if (message.header_.send_time_ < oldest_step) continue;
        result.insert(message.neuron_indexes_.cbegin(), message.neuron_indexes_.cend());
    }

    // Ignore extra neurons
    auto iter = result.lower_bound(output_size);
    result.erase(iter, result.end());
    return result;
}

}  // namespace knp::core::output
