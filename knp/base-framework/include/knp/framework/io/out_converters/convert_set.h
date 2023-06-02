/**
 * @brief Set converter header
 * @author Vartenkov Andrey
 * @date 01.06.2023
 */

#pragma once

#include <set>
#include <vector>

#include "knp/core/messaging/spike_message.h"


/**
 * @brief Network output processing namespace.
 */
namespace knp::framework::output
{
/**
 * @brief Get a set of all recently spiked neurons.
 * @param message_list spike messages.
 * @param output_size output population size, neuron indexes greater than output_size are ignored.
 * @return set of all neurons that spiked at a period defined by message_list.
 */
std::set<core::messaging::SpikeIndex> converter_to_set(
    const std::vector<core::messaging::SpikeMessage> &message_list, size_t output_size)
{
    std::set<core::messaging::SpikeIndex> result;
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
