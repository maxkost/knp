/**
 * @file data_storage.h
 * @brief Load data from file.
 * @author Vartenkov Andrey
 * @date 16.04.2024
 */
#pragma once

#include <knp/core/messaging/messaging.h>

#include <filesystem>
#include <vector>


namespace knp::framework
{
/**
 * @brief Convert messages to a vector that contains number of spikes per neuron index.
 * @param message_list input spike messages.
 * @param output_size size of the output vector. Neuron indexes greater than output_size are ignored.
 * @return vector such that index equals to the neuron index and value equals number of spikes for that neuron.
 */
std::vector<size_t> from_spikes_to_data(
    const std::vector<core::messaging::SpikeMessage> &message_list, size_t output_size);


/**
 * @brief Read spike messages from an HDF5 file.
 * @param path_to_h5 path to hdf5 data file.
 * @param uid sender UID.
 * @param time_per_step time per step. For example, spikes with timestamps 0 and time_per_step/2 will be a part of the
 * same message.
 * @return message vector, ordered by timestamp
 */
std::vector<core::messaging::SpikeMessage> load_messages_from_h5(
    const std::filesystem::path &path_to_h5, const knp::core::UID &uid, float time_per_step = 1.0f);


/**
 * @brief Save a vector of spike messages to HDF5 file
 * @param messages vector of spike messages to save into file.
 * @param path_to_save path to the file.
 * @param time_per_step time per step, use 1 if the unit is steps.
 * @note passing "messages" by value is not an error: it is sorted inside the function.
 */
void save_messages_to_h5(
    std::vector<core::messaging::SpikeMessage> messages, const std::filesystem::path &path_to_save,
    float time_per_step = 1.0f);


/**
 * @brief Read spike messages from a JSON file.
 * @param path_to_h5 path to .json data file.
 * @param uid sender UID.
 * @return message vector, ordered by timestamp.
 */
std::vector<core::messaging::SpikeMessage> load_messages_from_json(
    const std::filesystem::path &path_to_json, const knp::core::UID &uid);


/**
 * @brief Save a vector of spike messages to JSON file.
 * @param messages vector of spike messages to save into file.
 * @param path_to_save path to the file.
 * @note passing "messages" by value, they are sorted inside the function.
 */
void save_messages_to_json(
    std::vector<core::messaging::SpikeMessage> messages, const std::filesystem::path &path_to_save);
}  // namespace knp::framework
