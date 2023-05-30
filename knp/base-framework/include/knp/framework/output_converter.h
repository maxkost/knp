/**
 * @brief Output converter header
 * @author Vartenkov Andrey
 * @date 05.04.2023
 */

#pragma once
#include <functional>
#include <set>
#include <vector>

#include "knp/core/messaging/spike_message.h"

/**
 * @brief Network output processing namespace.
 */
namespace knp::framework::output
{
using MessageList = std::vector<core::messaging::SpikeMessage>;

/**
 * @brief a function to convert messages into data.
 * @tparam ResultType resulting data type.
 * @param MessageList list of spike messages.
 */
template <class ResultType>
using OutputConverter = std::function<ResultType(const MessageList &)>;


/**
 * @brief Convert a set of spike messages to a vector, so that result[neuron] = number of times neuron spiked.
 * @param message_list a list of spike messages.
 * @param output_size output population size.
 * @return vector of number of emitted spikes per neuron.
 * @details this converter(out_size = 6) will convert messages {0, 2}, {2, 4}, {1, 2} to (1, 1, 3, 0, 1, 0}.
 */
std::vector<size_t> converter_count(const MessageList &message_list, size_t output_size)
{
    std::vector<size_t> result(output_size, 0);
    for (auto &message : message_list)
    {
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
 * @param output_size output population size.
 * @return a function that converts spike messages to a bool vector of "true if the corresponding neuron spiked".
 * @details this converter(out_size=6) will convert messages {0, 2}, {2, 4}, {1, 2} to boolean vector {111010}.
 */
std::vector<bool> converter_bitwise(const MessageList &message_list, size_t output_size)
{
    std::vector<bool> result(output_size, false);
    for (auto &message : message_list)
    {
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
 * @return set of all neurons that spiked at a period defined by message_list.
 */
std::set<core::messaging::SpikeIndexType> converter_to_set(const MessageList &message_list, size_t output_size)
{
    std::set<core::messaging::SpikeIndexType> result;
    for (auto &message : message_list)
    {
        result.insert(message.neuron_indexes_.cbegin(), message.neuron_indexes_.cend());
    }

    // Ignore extra neurons
    auto iter = result.lower_bound(output_size);
    result.erase(iter, result.end());
    return result;
}


}  // namespace knp::framework::output
