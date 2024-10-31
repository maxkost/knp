/**
 * @file model_test.cpp
 * @brief Model class testing.
 * @kaspersky_support Artiom N.
 * @date 22.10.2023
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

#include <knp/framework/model.h>
#include <knp/framework/population/creators.h>
#include <knp/framework/projection/connectors.h>

#include <tests_common.h>

#include <exception>
#include <tuple>


using BLIFATParams = knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>;
using DeltaProjection = knp::core::Projection<knp::synapse_traits::DeltaSynapse>;
using Synapse = DeltaProjection::Synapse;


TEST(FrameworkSuite, ModelAddInputChannels)
{
    constexpr auto src_neurons_count = 5;
    constexpr auto dst_neurons_count = 3;

    auto src_pop{
        knp::framework::population::creators::make_random<knp::neuron_traits::BLIFATNeuron>(src_neurons_count)};
    auto dst_pop{
        knp::framework::population::creators::make_random<knp::neuron_traits::BLIFATNeuron>(dst_neurons_count)};

    knp::framework::Network network;

    network.add_population(src_pop);
    network.add_population(dst_pop);

    knp::framework::Model model(std::move(network));

    auto new_proj = knp::framework::projection::connect_populations<
        knp::synapse_traits::DeltaSynapse, decltype(src_pop)::PopulationNeuronType,
        decltype(dst_pop)::PopulationNeuronType>(src_pop, dst_pop);

    SPDLOG_DEBUG(
        "Presynaptic population UID = {}, postsynaptic population UID = {}, projection UID = {}.",
        std::string(src_pop.get_uid()), std::string(dst_pop.get_uid()), std::string(new_proj.get_uid()));

    EXPECT_THROW(
        model.connect_input_projection(knp::core::UID(), new_proj), std::logic_error);  //!OCLINT(False positive)
    EXPECT_THROW(
        model.add_input_channel(knp::core::UID(), new_proj.get_uid()), std::logic_error);  //!OCLINT(False positive)

    model.get_network().add_projection(new_proj);

    EXPECT_NO_THROW(model.connect_input_projection(knp::core::UID(), new_proj));  //!OCLINT(False positive)
    ASSERT_EQ(model.get_input_channels().size(), 1);
    EXPECT_NO_THROW(model.add_input_channel(knp::core::UID(), new_proj.get_uid()));  //!OCLINT(False positive)
    ASSERT_EQ(model.get_input_channels().size(), 2);
}


TEST(FrameworkSuite, ModelAddOutputChannels)  //!OCLINT(False positive)
{
    constexpr auto dst_neurons_count = 3;

    auto dst_pop{
        knp::framework::population::creators::make_random<knp::neuron_traits::BLIFATNeuron>(dst_neurons_count)};

    knp::framework::Network network;

    const auto pop_uid = dst_pop.get_uid();

    knp::framework::Model model(std::move(network));

    SPDLOG_DEBUG("Output population UID = {}.", std::string(pop_uid));

    EXPECT_THROW(
        model.connect_output_population(knp::core::UID(), dst_pop), std::logic_error);  //!OCLINT(False positive)
    EXPECT_THROW(
        model.add_output_channel(knp::core::UID(), dst_pop.get_uid()), std::logic_error);  //!OCLINT(False positive)

    model.get_network().add_population(dst_pop);

    EXPECT_NO_THROW(model.connect_output_population(knp::core::UID(), dst_pop));  //!OCLINT(False positive)
    ASSERT_EQ(model.get_output_channels().size(), 1);
    EXPECT_NO_THROW(model.add_output_channel(knp::core::UID(), pop_uid));  //!OCLINT(False positive)
    ASSERT_EQ(model.get_output_channels().size(), 2);
}
