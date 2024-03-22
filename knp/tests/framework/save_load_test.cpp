//
// Testing model save/load.
//


#include <knp/core/projection.h>
#include <knp/framework/save_network.h>

#include <generators.h>
#include <tests_common.h>

knp::framework::Network make_simple_network()
{
    namespace kt = knp::testing;
    // Create a single neuron network: input -> input_projection -> population <=> loop_projection

    kt::BLIFATPopulation population{kt::neuron_generator, 1};
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> loop_projection =
        kt::DeltaProjection{population.get_uid(), population.get_uid(), kt::synapse_generator, 1};
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> input_projection =
        kt::DeltaProjection{knp::core::UID{false}, population.get_uid(), kt::input_projection_gen, 1};
    knp::framework::Network network;
    network.add_population(population);
    network.add_projection(input_projection);
    network.add_projection(loop_projection);
    return network;
}


TEST(SaveLoadSuite, SaveTest)
{
    auto network = make_simple_network();
    knp::framework::save_network(network, "");
}
