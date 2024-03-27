#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/framework/network.h>
#include <knp/framework/save_network.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <bbp/sonata/config.h>
// Create an input projection

using DeltaProjection = knp::core::Projection<knp::synapse_traits::DeltaSynapse>;
using BLIFATPopulation = knp::core::Population<knp::neuron_traits::BLIFATNeuron>;

inline std::optional<DeltaProjection::Synapse> input_projection_gen(size_t /*index*/)  // NOLINT
{
    return DeltaProjection::Synapse{{1.0, 1, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};
}

// Create a loop projection
inline std::optional<DeltaProjection::Synapse> synapse_generator(size_t /*index*/)  // NOLINT
{
    return DeltaProjection::Synapse{{1.0, 6, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};
}


// Create population
inline knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron> neuron_generator(size_t)  // NOLINT
{
    return knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>{};
}

knp::framework::Network make_simple_network()
{
    // Create a single neuron network: input -> input_projection -> population <=> loop_projection
    BLIFATPopulation population{neuron_generator, 1};
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> loop_projection =
        DeltaProjection{population.get_uid(), population.get_uid(), synapse_generator, 1};
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> input_projection =
        DeltaProjection{knp::core::UID{false}, population.get_uid(), input_projection_gen, 1};
    knp::framework::Network network;
    network.add_population(population);
    network.add_projection(input_projection);
    network.add_projection(loop_projection);
    return network;
}


// int main()
//{
//     auto config = bbp::sonata::CircuitConfig::fromFile("network/network_config.json");
//     auto list_nodes = config.listNodePopulations();
//     for (auto &population_name : list_nodes)
//     {
//         auto snt_population = config.getNodePopulation(population_name);
//         std::cout << snt_population.size() << std::endl;
//     }
// }


int main()
{
    auto network = make_simple_network();
    knp::framework::save_network(network, "");
    auto network2 = knp::framework::load_network_alt("./network/network_config.json");
    std::cout << network2.populations_count() << " " << network2.projections_count() << std::endl;
}
