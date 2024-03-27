//
// Created by an_vartenkov on 31.01.24.
//

#pragma once
#include <knp/framework/network.h>

#include <filesystem>

namespace knp::framework
{
void save_network(const Network &network, const std::filesystem::path &dir);
Network load_network(const std::filesystem::path &config_path);
Network load_network_alt(const std::filesystem::path &config_path);

}  // namespace knp::framework
