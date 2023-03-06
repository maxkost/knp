//
// Projection tests
//
#include "knp/core/projection.h"

#include <tests_common.h>

#include <cstdlib>
#include <optional>

#include "knp/synapse-traits/delta.h"

namespace knc = knp::core;

using DeltaProjection = knc::Projection<knp::synapse_traits::DeltaSynapse>;
using Synapse = DeltaProjection::Synapse;
using SynapseParameters = DeltaProjection::SynapseParameters;
using SynapseGenerator = DeltaProjection::SynapseGenerator;


SynapseGenerator make_dense_generator(std::pair<size_t, size_t> pop_sizes, const SynapseParameters &default_params)
{
    SynapseGenerator generator = [=](size_t index) -> std::optional<Synapse>
    {
        const size_t id_from = index / pop_sizes.second;
        const size_t id_to = index % pop_sizes.second;
        return Synapse{default_params, id_from, id_to};
    };
    return generator;
}


SynapseGenerator make_cyclic_generator(std::pair<size_t, size_t> pop_sizes, const SynapseParameters &default_params)
{
    SynapseGenerator generator = [=](size_t index) -> std::optional<Synapse>
    {
        const size_t index_from = index % pop_sizes.first;
        return Synapse{default_params, index_from, (index_from + index / pop_sizes.second) % pop_sizes.second};
    };
    return generator;
}


TEST(ProjectionSuite, Generation)
{
    // Testing: constructor, get_connections, get_connection, operator[], get_size,
    const size_t presynaptic_size = 99;
    const size_t postsynaptic_size = 101;
    const float weight_constant = 0.0001;

    // Creating a dense projection with random weights ( -0.12 to +0.17 ) and delay (1 to 5)
    SynapseGenerator generator = [&](size_t iter) -> std::optional<Synapse>
    {
        const size_t id_from = iter / postsynaptic_size;
        const size_t id_to = iter % postsynaptic_size;
        const SynapseParameters params(weight_constant * iter, iter / 100 + 1);
        return std::optional(Synapse{params, id_from, id_to});
    };

    DeltaProjection projection(knc::UID{}, knc::UID{}, presynaptic_size * postsynaptic_size, generator);
    ASSERT_EQ(projection.size(), presynaptic_size * postsynaptic_size);
    ASSERT_EQ(projection[1000].params.delay_, 11);
    ASSERT_EQ(std::get<0>(projection.get_connection(1000)), 1000u / postsynaptic_size);
    ASSERT_EQ(projection.get_connections()[1000], projection.get_connection(1000));
}


TEST(ProjectionSuite, SynapseAddition)
{
    // TODO: add_synapses, remove_presynaptic_neuron, remove_postsynaptic_neuron, disconnect,
    const size_t presynaptic_size = 1000;
    const size_t postsynaptic_size = presynaptic_size;
    const size_t neuron_index = 10;  // A specific neuron

    // Create an empty projection
    DeltaProjection projection(knc::UID{}, knc::UID{});

    // Add synapses to an empty projection using a generator
    SynapseGenerator generator1 = [](size_t index) { return Synapse{{0.0F, 1}, index, index}; };
    projection.add_synapses(presynaptic_size, generator1);  // Add 1-to-1 synapses
    ASSERT_EQ(projection.size(), presynaptic_size);
    size_t count = projection.add_synapses(
        {Synapse{{1.F, 2}, neuron_index, neuron_index + 2}});  // Add a synapse from neuron 10 to neuron 12
    ASSERT_EQ(count, 1);

    // Add synapses to nonempty projection using a generator
    SynapseGenerator generator2 = [](size_t index) {
        return Synapse{{0.1F, 2}, index, (index + 1) % postsynaptic_size};
    };
    count = projection.add_synapses(presynaptic_size, generator2);  // Add synapses from pre neuron N to post neuron N+1
    ASSERT_EQ(count, presynaptic_size);
    ASSERT_EQ(projection.size(), 2 * presynaptic_size + 1);

    // Neuron #10 now should have three connections: 10, 11 and 12, let's check it
    std::vector<Synapse> connections;
    std::copy_if(
        projection.begin(), projection.end(), std::back_inserter(connections),
        [](const Synapse &syn) { return syn.id_from == neuron_index; });

    ASSERT_EQ(connections.size(), 3);
    for (size_t i = 0; i < 3; ++i)
    {
        // The ordering of values is implementation-defined, so we just check that all connections are present
        ASSERT_NE(
            std::find_if(
                connections.begin(), connections.end(),
                [&i](const Synapse &syn) { return syn.id_to == neuron_index + i; }),
            connections.end());
    }
}


TEST(ProjectionSuite, DeletePresynapticTest)
{
    const size_t size_from = 99;
    const size_t size_to = 101;
    const size_t synapses_per_neuron = 5;
    const size_t neuron_index = 10;
    auto generator = make_cyclic_generator({size_from, size_to}, {0.0F, 1});
    DeltaProjection projection{knc::UID{}, knc::UID{}, size_from * synapses_per_neuron, generator};
    const size_t count = projection.remove_presynaptic_neuron(neuron_index);
    ASSERT_EQ(count, synapses_per_neuron);
    ASSERT_EQ(projection.size(), (size_from - 1) * synapses_per_neuron);  // the size of projection is correct
    ASSERT_EQ(
        std::find_if(
            projection.begin(), projection.end(),
            [&](const Synapse &synapse) { return synapse.id_from == neuron_index; }),
        projection.end());  // all the synapses that should have been deleted are actually deleted
}


TEST(ProjectionSuite, DeletePostsynapticTest)
{
    const size_t size_from = 99;
    const size_t size_to = 101;
    const size_t synapses_per_neuron = 10;
    const size_t neuron_index = 15;
    auto generator = make_cyclic_generator({size_from, size_to}, {0.0F, 1});
    DeltaProjection projection{knc::UID{}, knc::UID{}, size_from * synapses_per_neuron, generator};
    const size_t count = projection.remove_postsynaptic_neuron(neuron_index);
    ASSERT_EQ(count, synapses_per_neuron);
    ASSERT_EQ(projection.size(), (size_from - 1) * synapses_per_neuron);  // the size of projection is correct
    ASSERT_EQ(
        std::find_if(
            projection.begin(), projection.end(),
            [&](const Synapse &synapse) { return synapse.id_to == neuron_index; }),
        projection.end());  // ensure all the synapses that should have been deleted are actually deleted
}


TEST(ProjectionSuite, SynapseRemoval)
{
    // Projection doesn't contain the neuron we're removing
    DeltaProjection projection{knc::UID{}, knc::UID{}};
    size_t count = projection.remove_presynaptic_neuron(100);
    ASSERT_EQ(count, 0);
    ASSERT_EQ(projection.size(), 0);

    const size_t presynaptic_size = 100;
    const size_t postsynaptic_size = presynaptic_size;
    const size_t synapses_per_neuron = 4;
    const size_t total_connections = presynaptic_size * synapses_per_neuron;

    // If we run this generator N * presynaptic size, we'll have 1 to N connections of x->x, x->x+1, ... x->x+N, cycled
    count = projection.add_synapses(
        total_connections, make_cyclic_generator({presynaptic_size, postsynaptic_size}, {0, 1}));
    ASSERT_EQ(count, total_connections);

    // Delete a single synapse
    projection.remove_synapse(0);
    ASSERT_EQ(projection.size(), total_connections - 1);  // A synapse is deleted
    ASSERT_EQ(projection[0].id_from, 1);                  // the deleted synapse is the correct one

    // Delete all synapses
    projection.clear();
    ASSERT_EQ(projection.size(), 0);
}


TEST(ProjectionSuite, LockTest)
{
    DeltaProjection projection(knc::UID{}, knc::UID{});
    ASSERT_FALSE(projection.is_locked());
    projection.lock_weights();
    ASSERT_TRUE(projection.is_locked());
    projection.unlock_weights();
    ASSERT_FALSE(projection.is_locked());
}


TEST(ProjectionSuite, DisconnectNeurons)
{
    const size_t presynaptic_size = 9;
    const size_t postsynaptic_size = 11;
    auto generator = make_dense_generator({presynaptic_size, postsynaptic_size}, {0.0, 1});
    DeltaProjection projection{knc::UID{}, knc::UID{}, presynaptic_size * postsynaptic_size, generator};
    const size_t count = projection.disconnect_neurons(0, 1);
    ASSERT_EQ(count, 1);
    ASSERT_EQ(
        std::count_if(
            projection.begin(), projection.end(), [](const Synapse &synapse) { return synapse.id_from == 0; }),
        postsynaptic_size - 1);
    ASSERT_EQ(
        std::count_if(projection.begin(), projection.end(), [](const Synapse &synapse) { return synapse.id_to == 1; }),
        presynaptic_size - 1);
}


TEST(ProjectionSuite, GetUIDTest)
{
    const knc::UID uid_from(true);
    const knc::UID uid_to(true);

    ASSERT_NE(uid_from, uid_to);
    auto generator = make_dense_generator({10, 10}, {0, 1});
    const DeltaProjection projection(uid_from, uid_to, 100, generator);
    ASSERT_EQ(projection.get_presynaptic(), uid_from);
    ASSERT_EQ(projection.get_postsynaptic(), uid_to);
}
