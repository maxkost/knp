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
    auto proj = knp::framework::projection::connectors::all_to_all<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), 3, 3);

    ASSERT_EQ(proj.size(), 9);
}


TEST(ProjectionConnectors, OneToOne)
{
    auto proj = knp::framework::projection::connectors::one_to_one<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), 5);

    ASSERT_EQ(proj.size(), 5);
}


TEST(ProjectionConnectors, FromContainer)
{
    const auto e_count = 5;
    std::vector<typename knp::core::Projection<knp::synapse_traits::DeltaSynapse>::Synapse> container;
    container.reserve(e_count);

    for (int i = 0; i < e_count; ++i)
    {
        container.push_back(std::make_tuple(
            knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters(), i, e_count - i));
    }

    auto proj = knp::framework::projection::connectors::from_container<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), container);
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

    auto proj = knp::framework::projection::connectors::from_map<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), syn_map);
}


TEST(ProjectionConnectors, FixedProbability)
{
    auto proj = knp::framework::projection::connectors::fixed_probability<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), 3, 5, 0.5);
}


TEST(ProjectionConnectors, IndexBased)
{
    auto proj = knp::framework::projection::connectors::index_based<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), 5, 3,
        [](size_t, size_t) {
            return std::make_optional<
                typename knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters>();
        });
}


TEST(ProjectionConnectors, FixedNumberPost)
{
    auto proj = knp::framework::projection::connectors::fixed_number_post<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), 3, 5, 2);
}


TEST(ProjectionConnectors, FixedNumberPre)
{
    auto proj = knp::framework::projection::connectors::fixed_number_pre<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), 3, 5, 2);
}


TEST(ProjectionConnectors, CloneProjection)
{
    auto proj = knp::framework::projection::connectors::one_to_one<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), 3);

    auto new_proj =
        knp::framework::projection::connectors::clone_projection<typename knp::synapse_traits::DeltaSynapse>(
            proj, [&proj](size_t index) { return std::get<knp::core::synapse_data>(proj[index]); });
}
