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
 * @brief Output channel namespace.
 */
namespace knp::framework::output
{
/**
 * @brief Get a set of recently spiked neuron indexes from the `message_list`.
 * @details The method ignores neuron indexes that are greater than the `output_size` value. 
 * @param message_list list of spike messages that contain indexes of spiked neurons.
 * @param output_size output vector size (usually corresponds to the size of an output population).
 * @return set of spiked neuron indexes.
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
