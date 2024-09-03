/**
 * @file projection_connectors_test.cpp
 * @brief Projection connectors tests.
 * @date 27.08.2024
 * @license Apache 2.0
 * @author Artiom N.
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/framework/projection/connectors.h>
#include <knp/synapse-traits/delta.h>

#include <tests_common.h>

#include <vector>


TEST(ProjectionConnectors, AllToAll)
{
    constexpr size_t src_pop_size = 3;
    constexpr size_t dest_pop_size = 3;

    auto proj = knp::framework::projection::connectors::all_to_all<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), src_pop_size, dest_pop_size);

    ASSERT_EQ(proj.size(), src_pop_size * dest_pop_size);

    std::map<int, std::vector<int>> conn_count;

    for (const auto& synapse : proj)
    {
        const auto target_syn_index = std::get<knp::core::target_neuron_id>(synapse);
        const auto source_syn_index = std::get<knp::core::source_neuron_id>(synapse);

        SPDLOG_DEBUG("Synapse: {} -> {}", target_syn_index, source_syn_index);
        conn_count[target_syn_index].push_back(source_syn_index);
    }

    for (const auto& [key, value] : conn_count)
    {
        ASSERT_EQ(value.size(), dest_pop_size);
    }
}


TEST(ProjectionConnectors, OneToOne)
{
    constexpr size_t pop_size = 5;

    auto proj = knp::framework::projection::connectors::one_to_one<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), pop_size);

    ASSERT_EQ(proj.size(), pop_size);

    std::map<int, std::vector<int>> conn_count;

    for (const auto& synapse : proj)
    {
        const auto target_syn_index = std::get<knp::core::target_neuron_id>(synapse);
        const auto source_syn_index = std::get<knp::core::source_neuron_id>(synapse);

        SPDLOG_DEBUG("Synapse: {} -> {}", target_syn_index, source_syn_index);
        conn_count[target_syn_index].push_back(source_syn_index);
    }

    for (const auto& [key, value] : conn_count)
    {
        ASSERT_EQ(value.size(), 1);
        ASSERT_EQ(key, value[0]);
    }
}


TEST(ProjectionConnectors, FromContainer)
{
    constexpr auto e_count = 5;
    std::vector<typename knp::core::Projection<knp::synapse_traits::DeltaSynapse>::Synapse> container;
    container.reserve(e_count);

    for (int i = 0; i < e_count; ++i)
    {
        container.push_back(std::make_tuple(
            knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters(), i, e_count - i));
    }

    auto proj =
        knp::framework::projection::connectors::from_container<typename knp::synapse_traits::DeltaSynapse, std::vector>(
            knp::core::UID(), knp::core::UID(), container);

    int i = 0;
    for (const auto& synapse : proj)
    {
        const auto container_syn = container[i++];
        ASSERT_EQ(std::get<knp::core::target_neuron_id>(synapse), std::get<knp::core::target_neuron_id>(container_syn));
        ASSERT_EQ(std::get<knp::core::source_neuron_id>(synapse), std::get<knp::core::source_neuron_id>(container_syn));
    }
}


TEST(ProjectionConnectors, FromMap)
{
    const auto e_count = 5;
    std::map<
        typename std::tuple<size_t, size_t>,
        typename knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters>
        syn_map;

    for (int i = 0; i < e_count; ++i)
    {
        syn_map[std::make_tuple(i, e_count - i)] =
            knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters();
    }

    auto proj = knp::framework::projection::connectors::from_map<typename knp::synapse_traits::DeltaSynapse, std::map>(
        knp::core::UID(), knp::core::UID(), syn_map);

    for (const auto& synapse : proj)
    {
        ASSERT_NE(
            syn_map.find(std::make_tuple(
                std::get<knp::core::source_neuron_id>(synapse), std::get<knp::core::target_neuron_id>(synapse))),
            syn_map.end());
    }
}


TEST(ProjectionConnectors, FixedProbability)
{
    auto proj = knp::framework::projection::connectors::fixed_probability<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), 3, 5, 0.5);
}


TEST(ProjectionConnectors, IndexBased)
{
    constexpr size_t src_pop_size = 5;
    constexpr size_t dest_pop_size = 3;

    auto proj = knp::framework::projection::connectors::index_based<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), src_pop_size, dest_pop_size,
        [dest_pop_size](size_t index0, size_t index1)
            -> std::optional<typename knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters>
        {
            // Diagonal.
            if (index0 == index1)
                return std::make_optional<
                    typename knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters>();
            return std::nullopt;
        });

    ASSERT_EQ(proj.size(), dest_pop_size);
}


TEST(ProjectionConnectors, FixedNumberPost)
{
    constexpr size_t src_pop_size = 3;
    constexpr size_t dest_pop_size = 5;
    constexpr size_t conn_count = 3;

    auto proj = knp::framework::projection::connectors::fixed_number_post<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), src_pop_size, dest_pop_size, conn_count);

    ASSERT_EQ(proj.size(), src_pop_size * conn_count);

    std::map<int, int> index_map;

    for (const auto& synapse : proj)
    {
        const auto target_syn_index = std::get<knp::core::target_neuron_id>(synapse);
        const auto source_syn_index = std::get<knp::core::source_neuron_id>(synapse);

        SPDLOG_DEBUG("Synapse: {} -> {}", target_syn_index, source_syn_index);

        ++index_map[source_syn_index];
    }

    for (const auto& [key, value] : index_map)
    {
        ASSERT_EQ(value, conn_count);
    }
}


TEST(ProjectionConnectors, FixedNumberPre)
{
    constexpr size_t src_pop_size = 4;
    constexpr size_t dest_pop_size = 8;
    constexpr size_t conn_count = 3;

    auto proj = knp::framework::projection::connectors::fixed_number_pre<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), src_pop_size, dest_pop_size, conn_count);

    ASSERT_EQ(proj.size(), dest_pop_size * conn_count);

    std::map<int, int> index_map;

    for (const auto& synapse : proj)
    {
        const auto target_syn_index = std::get<knp::core::target_neuron_id>(synapse);
        const auto source_syn_index = std::get<knp::core::source_neuron_id>(synapse);

        SPDLOG_DEBUG("Synapse: {} -> {}", target_syn_index, source_syn_index);

        ++index_map[target_syn_index];
    }

    for (const auto& [key, value] : index_map)
    {
        ASSERT_EQ(value, conn_count);
    }
}


TEST(ProjectionConnectors, CloneProjection)
{
    constexpr size_t pop_size = 3;

    auto proj = knp::framework::projection::connectors::one_to_one<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), pop_size);

    auto new_proj =
        knp::framework::projection::connectors::clone_projection<typename knp::synapse_traits::DeltaSynapse>(
            proj, [&proj](size_t index) { return std::get<knp::core::synapse_data>(proj[index]); });

    ASSERT_EQ(new_proj.size(), proj.size());

    for (int i = 0; i < proj.size(); ++i)
    {
        ASSERT_EQ(std::get<knp::core::target_neuron_id>(proj[i]), std::get<knp::core::target_neuron_id>(new_proj[i]));
        ASSERT_EQ(std::get<knp::core::source_neuron_id>(proj[i]), std::get<knp::core::source_neuron_id>(new_proj[i]));
    }
}
