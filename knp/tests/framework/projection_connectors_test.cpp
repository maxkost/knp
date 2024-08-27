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
}


TEST(ProjectionConnectors, OneToOne)
{
    auto proj = knp::framework::projection::connectors::one_to_one<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), 5);
}


TEST(ProjectionConnectors, FromContainer) {}


TEST(ProjectionConnectors, FixedProbability)
{
    auto proj = knp::framework::projection::connectors::fixed_probability<typename knp::synapse_traits::DeltaSynapse>(
        knp::core::UID(), knp::core::UID(), 3, 5, 0.5);
}


TEST(ProjectionConnectors, IndexBased)
{
    // auto proj = knp::framework::projection::connectors::index_based<typename
    // knp::synapse_traits::DeltaSynapse>(knp::core::UID(), knp::core::UID(), 3, 5, 0.5);
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
}
