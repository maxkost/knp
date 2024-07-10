/**
 * @file synchronization.cpp
 * @brief Getting network from backend.
 * @author Vartenkov Andrey.
 * @date 13.05.2024
 */

#include <knp/framework/network.h>

namespace knp::framework::synchronization
{
KNP_DECLSPEC Network get_network_copy(const knp::core::Backend &backend)
{
    auto data_ranges = backend.get_network_data();
    knp::framework::Network res_network;
    for (auto &iter = *data_ranges.population_range.first; iter != *data_ranges.population_range.second; ++iter)
    {
        auto population = *iter;
        res_network.add_population(std::move(population));
    }
    for (auto &iter = *data_ranges.projection_range.first; iter != *data_ranges.projection_range.second; ++iter)
    {
        auto projection = *iter;
        res_network.add_projection(std::move(projection));
    }
    return res_network;
}
}  // namespace knp::framework::synchronization
