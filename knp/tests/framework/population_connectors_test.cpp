/**
 * @file population_connectors_test.cpp
 * @brief Tests for population connectors.
 * @author Artiom N.
 * @date 08.08.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
        [dst_neurons_count](size_t index)
        {
            return std::make_tuple(
                knp::core::Projection<knp::synapse_traits::DeltaSynapse>::SynapseParameters(), index,
                index % dst_neurons_count);
        },
        src_neurons_count);

    ASSERT_EQ(new_proj.size(), src_neurons_count);
}
