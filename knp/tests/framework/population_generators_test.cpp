/**
 * @file population_generators_test.cpp
 * @brief Population generators tests.
 * @date 08.08.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/framework/population/generators.h>
#include <knp/neuron-traits/blifat.h>

#include <tests_common.h>

#include <vector>


TEST(PopulationGenerators, GenerateFromContainer)
{
    const typename std::vector<knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>> params{
        {1}, {2}, {3}};
    auto new_pop{knp::framework::population::from_container<std::vector, knp::neuron_traits::BLIFATNeuron>(params)};

    ASSERT_EQ(new_pop.size(), params.size());
    ASSERT_EQ(new_pop[0].n_time_steps_since_last_firing_, 1);
    ASSERT_EQ(new_pop[new_pop.size() - 1].n_time_steps_since_last_firing_, new_pop.size());
}


TEST(PopulationGenerators, GenerateRandom)
{
    constexpr auto neurons_count = 5;

    auto new_pop{knp::framework::population::make_random<knp::neuron_traits::BLIFATNeuron>(neurons_count)};

    ASSERT_EQ(new_pop.size(), neurons_count);
}


TEST(PopulationGenerators, GenerateDefault)
{
    constexpr auto neurons_count = 1;

    auto new_pop{knp::framework::population::make_default<knp::neuron_traits::BLIFATNeuron>(neurons_count)};

    ASSERT_EQ(new_pop.size(), neurons_count);
    ASSERT_EQ(
        new_pop[0].n_time_steps_since_last_firing_,
        knp::neuron_traits::default_values<knp::neuron_traits::BLIFATNeuron>::n_time_steps_since_last_firing_);
    ASSERT_EQ(
        new_pop[0].min_potential_,
        knp::neuron_traits::default_values<knp::neuron_traits::BLIFATNeuron>::min_potential_);
}
