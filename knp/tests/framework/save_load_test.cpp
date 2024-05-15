/**
 * @file save_load_test.cpp
 * @brief Test saving and building networks.
 * @author A. Vartenkov
 * @date 22.03.2024
 */


#include <knp/core/projection.h>
#include <knp/framework/sonata/network_io.h>

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


class SaveLoadNetworkSuite : public ::testing::Test
{
protected:
    void TearDown() override
    {
        std::filesystem::remove_all(path_to_network_ / "network");
        std::filesystem::remove(path_to_network_ / "config.json");
    }

    std::filesystem::path path_to_network_;
};


TEST_F(SaveLoadNetworkSuite, SaveTest)
{
    auto network = make_simple_network();
    path_to_network_ = ".";
    knp::framework::sonata::save_network(network, path_to_network_);
    ASSERT_TRUE(std::filesystem::is_directory(path_to_network_ / "network"));
    ASSERT_TRUE(std::filesystem::is_regular_file(path_to_network_ / "network/network_config.json"));
    ASSERT_TRUE(std::filesystem::is_regular_file(path_to_network_ / "network/populations.h5"));
    ASSERT_TRUE(std::filesystem::is_regular_file(path_to_network_ / "network/projections.h5"));
    ASSERT_TRUE(std::filesystem::is_regular_file(path_to_network_ / "network/neurons.csv"));
    ASSERT_TRUE(std::filesystem::is_regular_file(path_to_network_ / "network/synapses.csv"));
}


template <class Entity>
knp::core::UID get_uid(const Entity &entity)
{
    return std::visit([](const auto &v) { return v.get_uid(); }, entity);
}


// not efficient, doesn't matter.
template <class Content>
bool compare_container_contents(const Content &cont1, const Content &cont2, const knp::core::UID &uid)
{
    auto iter1 = std::find_if(cont1.begin(), cont1.end(), [&uid](const auto &value) { return get_uid(value) == uid; });
    auto iter2 = std::find_if(cont2.begin(), cont2.end(), [&uid](const auto &value) { return get_uid(value) == uid; });

    size_t size1 = std::visit([](auto &vec) { return vec.size(); }, *iter1);
    size_t size2 = std::visit([](auto &vec) { return vec.size(); }, *iter2);
    if (size1 != size2 || iter1->index() != iter2->index()) return false;
    // TODO: It would be nice to compare the contents, but parameter comparison is not yet implemented (1.04.24)
    return true;
}


// Comparing population vectors without taking order into account.
template <class Container>
bool are_similar_containers(const Container &cont1, const Container &cont2)
{
    std::set<knp::core::UID> uids1, uids2;
    for (const auto &v : cont1) uids1.insert(get_uid(v));
    for (const auto &v : cont2) uids2.insert(get_uid(v));
    if (uids1 != uids2) return false;

    if (!std::all_of(
            uids1.begin(), uids1.end(),
            [&cont1, &cont2](const knp::core::UID &uid) { return compare_container_contents(cont1, cont2, uid); }))
        return false;
    return true;
}


bool are_networks_similar(const knp::framework::Network &current, const knp::framework::Network &other)
{
    if (current.get_uid() != other.get_uid()) return false;

    if (!are_similar_containers(current.get_populations(), other.get_populations())) return false;
    if (!are_similar_containers(current.get_projections(), other.get_projections())) return false;
    return true;
}


TEST_F(SaveLoadNetworkSuite, SaveLoadTest)
{
    path_to_network_ = ".";
    auto network = make_simple_network();
    knp::framework::sonata::save_network(network, path_to_network_);
    auto network_loaded = knp::framework::sonata::load_network(path_to_network_);
    ASSERT_TRUE(are_networks_similar(network, network_loaded));
}
