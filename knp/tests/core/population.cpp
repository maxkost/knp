/**
 * Test file template
 */


#include <knp/core/population.h>
#include <knp/neuron-traits/blifat.h>
#include <tests_common.h>


using BLIFATParams = knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>;

BLIFATParams neuron_generator(knp::core::Population<knp::neuron_traits::BLIFATNeuron> &, size_t index)
{
    BLIFATParams p{.potential_ = static_cast<double>(index)};
    return p;
}


const auto neurons_count = 10;


TEST(PopulationSuite, Create)
{
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> population(neuron_generator, neurons_count);

    ASSERT_EQ(neurons_count, population.size());

    size_t n_counter = 0;
    for (const auto &neuron : population)
    {
        ASSERT_EQ(n_counter++, neuron.potential_);
    }
}


TEST(PopulationSuite, RemoveNeurons)
{
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> population(neuron_generator, neurons_count);

    ASSERT_EQ(neurons_count, population.size());

    const std::vector<size_t> indexes_to_remove{1, 3, 5};

    assert(indexes_to_remove.size() < neurons_count);

    population.remove_neurons(indexes_to_remove);

    for (size_t i = 0; i < neurons_count; ++i)
    {
        for (auto ni : indexes_to_remove) ASSERT_NE(population[i].potential_, ni);
    }
}


TEST(PopulationSuite, SetNeuronParameter)
{
    knp::core::Population<knp::neuron_traits::BLIFATNeuron> population(neuron_generator, neurons_count);

    ASSERT_EQ(neurons_count, population.size());

    constexpr size_t p_index = neurons_count / 2;

    population[p_index].potential_ = 150;

    ASSERT_EQ(150, population[p_index].potential_);
}
