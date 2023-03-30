/**
 * Network class testing.
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
        [](size_t index) -> BLIFATParams
        {
            BLIFATParams params{.potential_ = static_cast<double>(index / 2)};
            return params;
        },
        neurons_count);

    DeltaProjection projection1(
        knp::core::UID{}, knp::core::UID{}, synapses_count,
        [=](size_t index) -> std::optional<Synapse>
        {
            const size_t id_from = index;
            const size_t id_to = index;
            return Synapse{{}, id_from, id_to};
        });

    return std::make_tuple(population1, projection1);
}


TEST(FrameworkSuite, NetworkCreation)
{
    knp::framework::Network network;

    auto [population1, projection1] = create_entities();

    ASSERT_EQ(network.populations_count(), 0);
    ASSERT_EQ(network.projections_count(), 0);

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
    EXPECT_THROW(network.remove_population(uid_proj), std::runtime_error);
    ASSERT_EQ(network.populations_count(), 1);
    // Correct UID.
    network.remove_population(uid_pop);

    ASSERT_EQ(network.populations_count(), 0);

    // Incorrect UID.
    EXPECT_THROW(network.remove_projection(uid_pop), std::runtime_error);
    ASSERT_EQ(network.projections_count(), 1);
    // Correct UID.
    network.remove_projection(uid_proj);

    ASSERT_EQ(network.projections_count(), 0);
}
