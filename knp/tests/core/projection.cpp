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


TEST(CoreProjectionTest, ProjectionGenerationTest)
{
    // Testing: constructor, get_connections, get_connection, operator[], get_size,
    const size_t presynaptic_size = 99;
    const size_t postsynaptic_size = 101;

    // Creating a dense projection with random weights ( -0.12 to +0.17 ) and delay (1 to 5)
    SynapseGenerator generator =
        [](size_t iter) -> std::optional<Synapse>
        {
            const size_t id_from = iter / postsynaptic_size;
            const size_t id_to = iter % postsynaptic_size;
            const SynapseParameters params(0.0001 * iter, iter / 100 + 1);
            return std::optional(Synapse{params, id_from, id_to});
        };
    DeltaProjection projection(knc::UID{}, knc::UID{}, presynaptic_size * postsynaptic_size, generator);
    ASSERT_EQ(projection.size(), presynaptic_size * postsynaptic_size);
    ASSERT_EQ(projection[1000].params.delay_, 11);
    ASSERT_EQ(std::get<0>(projection.get_connection(1000)), 1000u / postsynaptic_size);
    ASSERT_EQ(projection.get_connections()[1000], projection.get_connection(1000));
}


TEST(CoreProjectionTest, ProjectionModificationTest)
{
    // TODO: add_synapses, remove_presynaptic_neuron, remove_postsynaptic_neuron, disconnect,
    // Create a 1-to-1 projection
    const size_t presynaptic_size = 1000;
    const size_t postsynaptic_size = 1000;
    DeltaProjection projection(knc::UID{}, knc::UID{});
    SynapseGenerator generator = [](size_t index) { return Synapse{{0.0F, 1}, index, index}; };
    projection.add_synapses(presynaptic_size, generator);
    ASSERT_EQ(projection.size(), presynaptic_size);
    projection.add_synapses({Synapse{{1.F, 2}, 10, 12}});
}