/**
 * @file data_storage_common.h
 * @brief Common functions and definitions for saving and loading data.
 * @author An. Vartenkov
 * @date 24.04.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
#pragma once
#include <knp/core/messaging/messaging.h>

#include <array>
#include <vector>


namespace knp::framework::io::storage::native
{
constexpr int MAGIC_NUMBER = 2682;
constexpr std::array<int64_t, 2> VERSION{0, 1};


std::vector<knp::core::messaging::SpikeMessage> convert_node_time_arrays_to_messages(
    const std::vector<int64_t> &nodes, const std::vector<float> &timestamps, const knp::core::UID &uid,
    float time_per_step);
}  // namespace knp::framework::io::storage::native
