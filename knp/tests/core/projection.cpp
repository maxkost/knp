//
// Projection tests
//
#include "knp/core/projection.h"
#include "knp/synapse-traits/delta.h"
#include <tests_common.h>
#include <cstdlib>
#include <optional>

namespace knc = knp::core;
namespace kntr = knp::synapse_traits;

using DeltaProjection = knc::Projection<kntr::DeltaSynapse>;
using Synapse = DeltaProjection::Synapse;
using SynapseParameters = DeltaProjection::SynapseParameters;
using SynapseGenerator = DeltaProjection::SynapseGenerator;


TEST(ProjectionSuite, Generation)
{
    // Testing: constructor, get_connections, get_connection, operator[], get_size,
    const size_t presynaptic_size = 99;
    const size_t postsynaptic_size = 101;

    // Creating a dense projection with random weights ( -0.12 to +0.17 ) and delay (1 to 5)
    SynapseGenerator generator =
            [&](size_t iter) -> std::optional<Synapse>
            {
                const size_t id_from = iter / postsynaptic_size;
                const size_t id_to = iter % postsynaptic_size;
                const SynapseParameters params(0.0001 * iter, iter / 100 + 1);
                return std::optional(Synapse{params, id_from, id_to});
            };

    DeltaProjection projection{knc::UID{}, knc::UID{}, presynaptic_size * postsynaptic_size, generator};
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
    const size_t neuron_index = 10; // A specific neuron
    DeltaProjection projection(knc::UID{}, knc::UID{}); // Create an empty projection

    SynapseGenerator generator1 = [](size_t index) { return Synapse{{0.0F, 1}, index, index}; };
    projection.add_synapses(presynaptic_size, generator1); // Add 1-to-1 synapses
    ASSERT_EQ(projection.size(), presynaptic_size);
    size_t count = projection.add_synapses({Synapse{{1.F, 2}, neuron_index, neuron_index + 2}}); // Add a synapse from neuron 10 to neuron 12
    ASSERT_EQ(count, 1);


    SynapseGenerator generator2 = [](size_t index) { return Synapse{{0.1F, 2}, index, (index+1) % postsynaptic_size}; };
    count = projection.add_synapses(presynaptic_size, generator2); // Add synapses from pre neuron N to post neuron N+1
    ASSERT_EQ(count, presynaptic_size);
    ASSERT_EQ(projection.size(), 2*presynaptic_size + 1);

    // Neuron #10 now should have three connections: 10, 11 and 12, let's check it
    std::vector<Synapse> connections;
    std::copy_if(projection.begin(), projection.end(), std::back_inserter(connections),
                 [](const Synapse &syn) { return syn.id_from == neuron_index; });

    ASSERT_EQ(connections.size(), 3);
    for (size_t i = 0; i < 3; ++i)
    {
        // The ordering of values is implementation-defined, so we just check that all connections are present
        ASSERT_TRUE(std::find_if(connections.begin(), connections.end(),
                                 [&i](const Synapse& syn) {return syn.id_to == neuron_index + i;})
                    != connections.end());
    }
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
    const size_t connections_per_neuron = 4;
    const size_t total_connections = presynaptic_size * connections_per_neuron;

    // If we run this generator N * presynaptic size, we'll have 1 to N connections of x->x, x->x+1, ... x->x+N, cycled
    SynapseGenerator generator = [&](size_t index) -> std::optional<Synapse>
        {
             const size_t index_from = index % presynaptic_size;
             return Synapse {{0, index / postsynaptic_size + 1}, index_from,
                             (index_from + index / postsynaptic_size) % postsynaptic_size};
        };
    count = projection.add_synapses(total_connections, generator);
    ASSERT_EQ(count, total_connections);

    // Now let's start deleting
    // Delete a single synapse
    projection.remove_synapse(0);
    ASSERT_EQ(projection.size(), total_connections - 1);  // A synapse is deleted
    ASSERT_EQ(projection[0].id_from, 1);  // the deleted synapse is the correct one

    // Delete a presynaptic neuron
    const size_t presynaptic_delete_index = 1;
    count = projection.remove_presynaptic_neuron(presynaptic_delete_index);
    ASSERT_EQ(count, connections_per_neuron);  // return value is as expected
    ASSERT_EQ(projection.size(), total_connections - 1 - connections_per_neuron);  // the size of projection is correct
    ASSERT_EQ(std::find_if(projection.begin(), projection.end(),
                           [&](Synapse &synapse) { return synapse.id_from == presynaptic_delete_index; }),
              projection.end());  // all the synapses that should have been deleted are actually deleted

    // Delete a postsynaptic_neuron
    const size_t postsynaptic_delete_index = presynaptic_delete_index;
    count = projection.remove_postsynaptic_neuron(postsynaptic_delete_index);
    ASSERT_EQ(count, connections_per_neuron - 1); // we've already deleted x-x connection, so one less is left
    ASSERT_EQ(std::find_if(projection.begin(), projection.end(),
                           [&](Synapse &synapse) { return synapse.id_to == postsynaptic_delete_index; }),
              projection.end());

    // Delete all
    projection.clear();
    ASSERT_EQ(projection.size(), 0);
}

