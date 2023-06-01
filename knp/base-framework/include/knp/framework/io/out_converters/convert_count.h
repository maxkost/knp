/**
 * @brief Counting converter header
 * @author Vartenkov Andrey
 * @date 01.06.2023
 */

#pragma once

#include <vector>

#include "knp/core/messaging/spike_message.h"


/**
 * @brief Network output processing namespace.
 */
namespace knp::framework::output
{
/**
 * @brief Convert a set of spike messages to a vector, so that result[neuron] = number of times neuron spiked.
 * @param message_list a list of spike messages.
 * @param output_size output population size.
 * @return vector of number of emitted spikes per neuron.
 * @details this converter(out_size = 6) will convert messages {0, 2}, {2, 4}, {1, 2} to (1, 1, 3, 0, 1, 0}.
 */
std::vector<size_t> converter_count(const std::vector<core::messaging::SpikeMessage> &message_list, size_t output_size)
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
}  // namespace knp::framework::output
