/**
 * Network class testing.
 */

#include <knp/framework/network.h>

#include <tests_common.h>


using BLIFATParams = knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>;
using DeltaProjection = knp::core::Projection<knp::synapse_traits::DeltaSynapse>;
using Synapse = DeltaProjection::Synapse;

const auto neurons_count = 10;
const auto synapses_count = 10;


TEST(FrameworkSuite, NetworkCreation)
{
    knp::framework::Network network;

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

    network.add_population(std::move(population1));
    network.add_projection(std::move(projection1));
}
