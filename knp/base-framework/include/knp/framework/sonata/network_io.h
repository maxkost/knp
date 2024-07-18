/**
 * @file network_io.h
 * @brief Saving and loading networks.
 * @author A. Vartenkov
 * @date 31.01.2024
 */

#pragma once
#include <knp/core/impexp.h>
#include <knp/framework/network.h>

#include <filesystem>

/**
 * @brief SONATA namespace.
 */
namespace knp::framework::sonata
{
/**
 * @brief Save network to disk.
 * @note The network is saved in the SONATA format.
 * @param network network to save.
 * @param dir directory to save the network.
 */
KNP_DECLSPEC void save_network(const Network &network, const std::filesystem::path &dir);


/**
 * @brief Load network from disk.
 * @param config_path path to network configuration file.
 * @return loaded network.
 */
KNP_DECLSPEC Network load_network(const std::filesystem::path &config_path);

}  // namespace knp::framework::sonata
