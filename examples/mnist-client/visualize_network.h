/**
 * @file visualize_network.h
 * @brief Functions for graph visualization.
 * @author A. Vartenkov
 * @date 26.07.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once
#include <knp/framework/network.h>

#include <vector>

#include <opencv2/core/types.hpp>


/**
 * Network description for drawing. You can use this to check network structure.
 */
struct NetworkGraph
{
public:
    struct Node
    {
        size_t size_;
        knp::core::UID uid_;
        size_t type_;
    };
    std::vector<Node> nodes_;

    struct Edge
    {
        size_t size_;
        int index_from_;
        int index_to_;
        knp::core::UID uid_;
        size_t type_;
    };
    std::vector<Edge> edges_;

    explicit NetworkGraph(const knp::framework::Network &network);
};


/**
 * @brief Print nodes and edges connections.
 * @param graph network graph.
 */
void print_network_description(const NetworkGraph &graph);


/**
 * @brief Print whole network information, though not in a very human-friendly manner.
 * @param graph network graph.
 */
void print_modified_network_description(const NetworkGraph &graph);

/**
 * @brief If the
 * @param graph
 * @return
 */
std::vector<std::vector<int>> divide_graph_by_connectivity(const NetworkGraph &graph);

std::vector<cv::Point2i> position_network(
    const NetworkGraph &graph, const std::vector<int> &nodes, cv::Size screen_size, int margin, int num_iterations);

void position_network_test(
    const NetworkGraph &graph, const std::vector<int> &nodes, const cv::Size &screen_size, int margin = 50);
