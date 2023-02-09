//
// Projection tests
//
#include "knp/core/projection.h"
#include "knp/synapse-traits/delta.h"
#include <tests_common.h>
#include <cstdlib>
#include <optional>


TEST(CoreProjectionTest, ProjectionGenerationTest)
{
    namespace knc = knp::core;
    namespace kntr = knp::synapse_traits;

    const size_t presynaptic_size = 99;
    const size_t postsynaptic_size = 101;

    using DeltaProjection = knc::Projection<kntr::DeltaSynapse>;
    using Synapse = DeltaProjection::Synapse;
    using SynapseParameters = DeltaProjection::SynapseParameters;
    using SynapseGenerator = DeltaProjection::SynapseGenerator;

    // Creating a dense projection with random weights ( -0.12 to +0.17 ) and delay (1 to 5)
    SynapseGenerator generator =
        [](size_t iter) -> std::optional<Synapse>
        {
            const size_t id_from = iter / postsynaptic_size;
            const size_t id_to = iter % postsynaptic_size;
            SynapseParameters params;
            params.delay_ = std::rand() % 5 + 1;
            params.weight_ = 0.01 * (std::rand() % 30 - 12);
            return std::optional(Synapse{params, id_from, id_to});
        };
    DeltaProjection projection(knc::UID{}, knc::UID{}, presynaptic_size * postsynaptic_size, generator);
    ASSERT_EQ(projection.get_size(), presynaptic_size * postsynaptic_size);
    size_t synapses_removed = projection.remove_postsynaptic_neuron(100);
    ASSERT_EQ(synapses_removed, presynaptic_size);
}