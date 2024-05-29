/**
 * @file network_test.cpp
 * @brief Network class testing.
 * @author A. Vartenkov
 * @date 12.02.2024
 */

#include <knp/framework/network.h>

#include <tests_common.h>

#include <tuple>


using BLIFATParams = knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>;
using DeltaProjection = knp::core::Projection<knp::synapse_traits::DeltaSynapse>;
using Synapse = DeltaProjection::Synapse;

const auto neurons_count = 10;
const auto synapses_count = 10;


auto create_entities()
{
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> population1(
        [=](size_t index) -> BLIFATParams
        {
            BLIFATParams params;
            params.potential_ = static_cast<double>(index / 2);
            return params;
        },
        neurons_count);

    DeltaProjection projection1(
        knp::core::UID{}, knp::core::UID{},
        [=](size_t index) -> std::optional<Synapse>
        {
            const uint32_t id_from = index;
            const uint32_t id_to = index;
            return Synapse{{}, id_from, id_to};
        },
        synapses_count);

    return std::make_tuple(population1, projection1);
}


TEST(FrameworkSuite, EmptyNetwork)
{
    // Testing default network constructor: should create an empty network
    knp::framework::Network network;
    // Number of projections in an empty network is zero
    ASSERT_EQ(network.projections_count(), 0);
    // Number of populations in an empty network is zero
    ASSERT_EQ(network.populations_count(), 0);
    // Check iterator correctness
    ASSERT_EQ(network.begin_populations(), network.end_populations());
    ASSERT_EQ(network.begin_projections(), network.end_projections());
}


TEST(FrameworkSuite, NetworkCreation)
{
    knp::framework::Network network;
    auto [population1, projection1] = create_entities();

    network.add_population(std::move(population1));
    ASSERT_EQ(network.populations_count(), 1);
    ASSERT_EQ(network.projections_count(), 0);

    network.add_projection(std::move(projection1));
    ASSERT_EQ(network.populations_count(), 1);
    ASSERT_EQ(network.projections_count(), 1);
}


TEST(FrameworkSuite, NetworkRemoveEntities)
{
    knp::framework::Network network;

    auto [population1, projection1] = create_entities();

    auto [uid_pop, uid_proj] = std::make_tuple(population1.get_uid(), projection1.get_uid());

    ASSERT_EQ(network.populations_count(), 0);
    ASSERT_EQ(network.projections_count(), 0);

    network.add_population(std::move(population1));
    network.add_projection(std::move(projection1));

    ASSERT_EQ(network.populations_count(), 1);
    ASSERT_EQ(network.projections_count(), 1);

    // Incorrect UID.
    EXPECT_THROW(network.remove_population(uid_proj), std::runtime_error);  // !OCLint(no "non case label")
    ASSERT_EQ(network.populations_count(), 1);
    // Correct UID.
    network.remove_population(uid_pop);

    ASSERT_EQ(network.populations_count(), 0);

    // Incorrect UID.
    EXPECT_THROW(network.remove_projection(uid_pop), std::runtime_error);  // !OCLint(no "non case label")
    ASSERT_EQ(network.projections_count(), 1);
    // Correct UID.
    network.remove_projection(uid_proj);

    ASSERT_EQ(network.projections_count(), 0);
}
