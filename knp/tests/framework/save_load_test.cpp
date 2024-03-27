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


TEST(SaveLoadSuit, SaveTest)
{
    auto network = make_simple_network();
    knp::framework::save_network(network, "");
    ASSERT_TRUE(std::filesystem::is_directory("network"));
    ASSERT_TRUE(std::filesystem::is_regular_file("network/network_config.json"));
    ASSERT_TRUE(std::filesystem::is_regular_file("network/populations.h5"));
    ASSERT_TRUE(std::filesystem::is_regular_file("network/projections.h5"));
    ASSERT_TRUE(std::filesystem::is_regular_file("network/neurons.csv"));
    ASSERT_TRUE(std::filesystem::is_regular_file("network/synapses.csv"));
}


TEST(SaveLoadSuite, SaveLoadTest)
{
    auto network = make_simple_network();
    knp::framework::save_network(network, "");
    auto network_loaded = knp::framework::load_network_alt("network/network_config.json");
    ASSERT_EQ(network.populations_count(), network_loaded.populations_count());
    ASSERT_EQ(network.projections_count(), network_loaded.projections_count());
    knp::core::UID uid_from = std::visit([](auto &proj) { return proj.get_uid(); }, *network.begin_projections());
    knp::core::UID uid_to = std::visit([](auto &proj) { return proj.get_uid(); }, *network_loaded.begin_projections());
    ASSERT_EQ(uid_from, uid_to);
    uid_from = std::visit([](auto &proj) { return proj.get_uid(); }, *network.begin_populations());
    uid_to = std::visit([](auto &proj) { return proj.get_uid(); }, *network_loaded.begin_populations());
    ASSERT_EQ(uid_from, uid_to);
}
