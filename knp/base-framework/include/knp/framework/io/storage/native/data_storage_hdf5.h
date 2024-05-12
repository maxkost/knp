/**
 * @file data_storage_hdf5.h
 * @brief Load data from file.
 * @author Vartenkov Andrey
 * @date 22.04.2024
 */
#pragma once

#include <knp/core/messaging/messaging.h>

#include <filesystem>
#include <vector>


/**
 * @brief Data storage namespace.
 */
namespace knp::framework::storage::native
{
/**
 * @brief Read spike messages from an HDF5 file.
 * @note Spike messages with timestamps 0 and time_per_step/2 will be a part of the
 * same message.
 * @param path_to_h5 path to HDF5 data file.
 * @param uid sender UID.
 * @param time_per_step time per step. 
 * @param strict_format if `true`, method throws exception on wrong format.
 * @return vector of messages sorted by timestamps.
 */
std::vector<core::messaging::SpikeMessage> load_messages_from_h5(
    const std::filesystem::path &path_to_h5, const knp::core::UID &uid, float time_per_step = 1.0f,
    bool strict_format = true);


/**
 * @brief Save a vector of spike messages to HDF5 file.
 * @param messages vector of spike messages to save.
 * @param path_to_save path to file.
 * @param time_per_step time per step, use 1 if the unit is steps.
 * @note Passing "messages" by value is not an error: it is sorted inside the function.
 */
void save_messages_to_h5(
    std::vector<core::messaging::SpikeMessage> messages, const std::filesystem::path &path_to_save,
    float time_per_step = 1.0f);

}  // namespace knp::framework::storage::native
