/**
 * @file convert_count.h
 * @brief Header for counting converter.
 * @author Vartenkov Andrey
 * @date 01.06.2023
 */

#pragma once

#include <knp/core/messaging/spike_message.h>

#include <vector>


/**
 * @brief Output channel namespace.
 */
namespace knp::framework::output
{
/**
 * @brief Convert a set of spike messages to a vector where an element value is equal to the number of times that a
 * neuron with the corresponding index has spiked.
 * @details For example, a method, where `output_size` equals 6 and `message_list` contains messages `{0, 2}`, `{2, 4}`,
 * `{1, 2}`, will return a vector `{1, 1, 3, 0, 1, 0}`.
 * @param message_list list of spike messages that contain indexes of spiked neurons.
 * @param output_size output vector size (usually corresponds to the size of an output population).
 * @return vector.
 */
std::vector<size_t> converter_count(const std::vector<core::messaging::SpikeMessage> &message_list, size_t output_size)
{
    std::vector<size_t> result(output_size, 0);
    for (const auto &message : message_list)
    {
        for (auto index : message.neuron_indexes_)
        {
            if (index < output_size) ++result[index];
        }
    }
    return result;
}
}  // namespace knp::framework::output
