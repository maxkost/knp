/**
 * @file network_sonata_io.h
 * @brief Saving and loading networks.
 * @author A. Vartenkov
 * @date 31.01.2024
 */

#pragma once
#include <knp/framework/network.h>

#include <filesystem>

/**
 * @brief Framework namespace.
 */
namespace knp::framework::sonata
{
/**
 * @brief Save network to the disk. Sonata format.
 * @param network network to be saved.
 * @param dir directory where the network is saved.
 */
void save_network(const Network &network, const std::filesystem::path &dir);


/**
 * @brief Load network from disk.
 * @param config_path path to network configuration file.
 * @return loaded network.
 */
Network load_network(const std::filesystem::path &config_path);

}  // namespace knp::framework::sonata
