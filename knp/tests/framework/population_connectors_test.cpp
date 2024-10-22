/**
 * @file population_connectors_test.cpp
 * @brief Tests for population connectors.
 * @author Artiom N.
 * @date 08.08.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/framework/population/creators.h>
#include <knp/framework/projection/connectors.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <tests_common.h>

#include <vector>


TEST(PopulationConnectors, AllToAllConnector)
{
    constexpr auto src_neurons_count = 5;
    constexpr auto dst_neurons_count = 3;

    auto src_pop{
        knp::framework::population::creators::make_random<knp::neuron_traits::BLIFATNeuron>(src_neurons_count)};
    auto dst_pop{
        knp::framework::population::creators::make_random<knp::neuron_traits::BLIFATNeuron>(dst_neurons_count)};

    auto new_proj = knp::framework::projection::connect_populations<
        knp::synapse_traits::DeltaSynapse, decltype(src_pop)::PopulationNeuronType,
        decltype(dst_pop)::PopulationNeuronType>(src_pop, dst_pop);

    ASSERT_EQ(new_proj.size(), src_neurons_count * dst_neurons_count);
}


TEST(PopulationConnectors, ArbitraryConnector)
{
    constexpr auto src_neurons_count = 5;
    constexpr auto dst_neurons_count = 3;

    auto src_pop{
        knp::framework::population::creators::make_random<knp::neuron_traits::BLIFATNeuron>(src_neurons_count)};
    auto dst_pop{
        knp::framework::population::creators::make_random<knp::neuron_traits::BLIFATNeuron>(dst_neurons_count)};

    auto new_proj = knp::framework::projection::connect_populations<
        knp::synapse_traits::DeltaSynapse, decltype(src_pop)::PopulationNeuronType,
        decltype(dst_pop)::PopulationNeuronType>(
        src_pop, dst_pop,
        [](size_t index)
        {
            return std::make_tuple(
                knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters(), index,
                index % dst_neurons_count);
        },
        src_neurons_count);

    ASSERT_EQ(new_proj.size(), src_neurons_count);
}
