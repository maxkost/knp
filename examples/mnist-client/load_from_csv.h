/**
 * @file load_from_csv.h
 * @brief Loading network from csv-based ARNI format.
 * @date 18.09.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */


#pragma once
#include <knp/core/uid.h>
#include <knp/framework/network.h>

#include <filesystem>
#include <string>
#include <utility>
#include <vector>


/**
 * @brief Load network from a specific CSV file format.
 * @param monitoring_file network data file.
 * @param tact training tact if the file contains a number of network states.
 * @param excluded_population_names if there are any populations that shouldn't be loaded.
 * @param input_projection_uids output parameter: which projections are input projections.
 * @return loaded network.
 */
knp::framework::Network create_network_from_monitoring_file(
    const std::filesystem::path &monitoring_file, int tact, const std::vector<std::string> &excluded_population_names,
    std::vector<std::pair<knp::core::UID, size_t>> &input_projection_uids);
