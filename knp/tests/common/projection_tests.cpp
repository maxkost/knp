//
// Projection tests
//
#include <knp/core/projection.h>
#include <knp/synapse-traits/delta.h>
#include "tests_common.h"
#include <cstdlib>
#include <optional>


TEST(CoreProjectionTest, ProjectionGenerationTest)
{
    namespace knc = knp::core;
    namespace kntr = knp::synapse_traits;

    const size_t presynaptic_size = 99;
    const size_t postsynaptic_size = 101;

    // Creating a dense projection with random weights ( -0.12 to +0.17 ) and delay (1 to 5)
    const std::function<std::optional<knc::SynapseValue<kntr::DeltaSynapse>>(size_t)> generator =
        [](size_t iter) -> std::optional<knc::SynapseValue<kntr::DeltaSynapse>>
        {
            const size_t id_from = iter / presynaptic_size;
            const size_t id_to = iter % postsynaptic_size;
            knp::core::SynapseParameters<kntr::DeltaSynapse> params;
            params.delay_ = std::rand() % 5 + 1;
            params.weight_ = 0.01 * (std::rand() % 30 - 12);
            return std::optional(knc::SynapseValue<kntr::DeltaSynapse>{params, id_from, id_to});
        };
    knc::Projection<knp::synapse_traits::DeltaSynapse> projection(knc::UID{}, knc::UID{},
                                                                  presynaptic_size * postsynaptic_size, generator);
    ASSERT_EQ(projection.get_size(), presynaptic_size * postsynaptic_size);

}