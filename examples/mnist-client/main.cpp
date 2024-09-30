/**
 * @file main.cpp
 * @brief An in-progress demo program for building and running MNIST-classification spiking network.
 * @author A. Vartenkov
 * @date 26.07.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/framework/io/input_channel.h>
#include <knp/framework/monitoring/observer.h>
#include <knp/framework/network.h>
#include <knp/framework/sonata/network_io.h>

#include <iostream>

#include <boost/program_options.hpp>

#include "inference.h"
#include "visualize_network.h"


// Namespace for program options.
namespace po = boost::program_options;


// Load network and show or run it depending on command line parameters.
// On task=show loads a network and shows a subgraph of it.
// On task=infer loads and runs network using data from a binary data file.
int main(int argc, char **argv)
{
    knp::core::UID output_uid;
    std::string task;
    std::string path_to_network, path_to_data;
    po::options_description options;
    options.add_options()("help,h", "Produce help message.")(
        "task,t", po::value(&task), "Type of task: show, train, infer.")(
        "net-path,p", po::value(&path_to_network), "File or directory for network storage.")(
        "data-path,d", po::value(&path_to_data), "File for data storage.");
    po::variables_map options_map;
    po::store(po::parse_command_line(argc, argv, options), options_map);
    po::notify(options_map);

    if (!options_map.count("net-path"))
    {
        std::cout << "No \"net-path\" parameter required to load network. Stopping the program..." << std::endl;
        return 0;
    }

    if (task == "show")
    {
        const auto network_path = options_map["net-path"].as<std::string>();
        knp::framework::Network network = knp::framework::sonata::load_network(network_path);
        const NetworkGraph net_graph(network);
        print_network_description(net_graph);
        // Press ESC to exit
        position_network_test(NetworkGraph(network), divide_graph_by_connectivity(net_graph)[0], {1000, 700});
    }

    if (task == "infer")
    {
        std::filesystem::path path_to_backend =
            std::filesystem::path(argv[0]).parent_path() / "knp-cpu-single-threaded-backend";
        do_inference(
            options_map["net-path"].as<std::string>(), options_map["data-path"].as<std::string>(), path_to_backend);
    }
}
