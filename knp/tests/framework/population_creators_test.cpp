/**
 * @file population_creators_test.cpp
 * @brief Tests for population creators.
 * @date 08.08.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/framework/population/creators.h>
#include <knp/neuron-traits/blifat.h>

#include <tests_common.h>

#include <vector>


TEST(PopulationGenerators, CreatorFromContainer)
{
    const typename std::vector<knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>> params{
        {1}, {2}, {3}};
    auto new_pop{
        knp::framework::population::creators::from_container<knp::neuron_traits::BLIFATNeuron, std::vector>(params)};

    ASSERT_EQ(new_pop.size(), params.size());
    ASSERT_EQ(new_pop[0].n_time_steps_since_last_firing_, 1);
    ASSERT_EQ(new_pop[new_pop.size() - 1].n_time_steps_since_last_firing_, new_pop.size());
}


TEST(PopulationGenerators, CreatorRandom)
{
    constexpr auto neurons_count = 5;

    auto new_pop{knp::framework::population::creators::make_random<knp::neuron_traits::BLIFATNeuron>(neurons_count)};

    ASSERT_EQ(new_pop.size(), neurons_count);
}


TEST(PopulationGenerators, CreatorDefault)
{
    constexpr auto neurons_count = 1;

    auto new_pop{knp::framework::population::creators::make_default<knp::neuron_traits::BLIFATNeuron>(neurons_count)};

    ASSERT_EQ(new_pop.size(), neurons_count);
    ASSERT_EQ(
        new_pop[0].n_time_steps_since_last_firing_,
        knp::neuron_traits::default_values<knp::neuron_traits::BLIFATNeuron>::n_time_steps_since_last_firing_);
    ASSERT_EQ(
        new_pop[0].min_potential_,
        knp::neuron_traits::default_values<knp::neuron_traits::BLIFATNeuron>::min_potential_);
}


TEST(PopulationGenerators, CreatorCopy)
{
    constexpr auto neurons_count = 1;

    knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron> source_neuron;

    source_neuron.absolute_refractory_period_ = 12345;

    auto new_pop{knp::framework::population::creators::make_copy<knp::neuron_traits::BLIFATNeuron>(
        neurons_count, source_neuron)};

    ASSERT_EQ(new_pop.size(), neurons_count);
    ASSERT_EQ(new_pop[0].absolute_refractory_period_, source_neuron.absolute_refractory_period_);
}
