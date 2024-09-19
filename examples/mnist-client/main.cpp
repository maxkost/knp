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
#include <vector>

#include <boost/program_options.hpp>

#include "inference.h"
#include "load_from_csv.h"
#include "visualize_network.h"


namespace po = boost::program_options;


int main(int argc, char **argv)
{
    knp::core::UID output_uid;
    std::string task;
    std::string path_to_network, path_to_data;
    po::options_description options;
    options.add_options()("help,h", "Produce help message")(
        "task,t", po::value(&task), "Type of task: show, train, infer")(
        "net-path,p", po::value(&path_to_network), "File or directory for network storage.")(
        "data-path,d", po::value(&path_to_data), "File for data storage.");
    po::variables_map options_map;
    po::store(po::parse_command_line(argc, argv, options), options_map);
    po::notify(options_map);

    if (!options_map.count("net-path")) return 0;

    if (task == "show")
    {
        std::vector<std::pair<knp::core::UID, size_t>> input_uids;
        knp::framework::Network network =
            create_network_from_monitoring_file(options_map["net-path"].as<std::string>(), 0, {}, input_uids);

        std::filesystem::create_directory("mnist_network");
        knp::framework::sonata::save_network(network, "mnist_network");
        std::cout << "Loaded" << std::endl;
        print_network_description(NetworkGraph(network));

        // Press ESC to exit or any key to iterate graph
        position_network_test(NetworkGraph(network), {0, 1, 2, 3, 4}, {1000, 700});
    }

    if (task == "infer")
    {
        std::filesystem::path path_to_backend =
            std::filesystem::path(argv[0]).parent_path() / "knp-cpu-single-threaded-backend";
        do_inference(
            options_map["net-path"].as<std::string>(), options_map["data-path"].as<std::string>(), path_to_backend,
            1200000);
    }
}
