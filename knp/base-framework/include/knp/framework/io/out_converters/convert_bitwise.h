/**
 * @brief Bitwise converter header
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
 * @brief Convert messages to bool vector where an element is true if a corresponding neuron sent at least one spike.
 * @param message_list spike messages.
 * @param output_size output population size.
 * @return a function that converts spike messages to a bool vector of "true if the corresponding neuron spiked".
 * @details this converter(out_size=6) will convert messages {0, 2}, {2, 4}, {1, 2} to boolean vector {111010}.
 */
std::vector<bool> converter_bitwise(const std::vector<core::messaging::SpikeMessage> &message_list, size_t output_size)
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
}  // namespace knp::framework::output
