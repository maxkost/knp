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


namespace knp::framework::storage::native
{

/**
 * @brief Read spike messages from a JSON file.
 * @param path_to_json path to .json data file.
 * @param uid sender UID.
 * @param strict_format if to throw exception on wrong format.
 * @return message vector, ordered by timestamp.
 */
std::vector<core::messaging::SpikeMessage> load_messages_from_json(
    const std::filesystem::path &path_to_json, const knp::core::UID &uid, bool strict_format = true);


/**
 * @brief Save a vector of spike messages to JSON file.
 * @param messages vector of spike messages to save into file.
 * @param path_to_save path to the file.
 * @note passing "messages" by value, they are sorted inside the function.
 */
void save_messages_to_json(
    std::vector<core::messaging::SpikeMessage> messages, const std::filesystem::path &path_to_save);
}  // namespace knp::framework::storage::native
