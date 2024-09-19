/**
 * @file visualize_network.h
 * @brief Functions for graph visualization. Warning: most of them are not well-tested or stable yet.
 * @date 26.07.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once
#include <knp/framework/network.h>

#include <string>
#include <vector>

#include <opencv2/core/types.hpp>


/**
 * Network description for drawing. You can use this to check network structure.
 */
struct NetworkGraph
{
public:
    /**
     * @brief Node description structure.
     */
    struct Node
    {
        // cppcheck-suppress unusedStructMember
        size_t size_;
        knp::core::UID uid_;
        std::string name_;
        // cppcheck-suppress unusedStructMember
        size_t type_;
    };
    std::vector<Node> nodes_;

    /**
     * @brief edge description structure
     */
    struct Edge
    {
        // cppcheck-suppress unusedStructMember
        size_t size_;
        // cppcheck-suppress unusedStructMember
        int index_from_;
        // cppcheck-suppress unusedStructMember
        int index_to_;
        knp::core::UID uid_;
        std::string name_;
        // cppcheck-suppress unusedStructMember
        size_t type_;
    };
    std::vector<Edge> edges_;

    /**
     * @brief Build network graph from a network.
     * @param network
     */
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
 * @brief Divide network graph into subgraphs.
 * @param graph network graph.
 * @return vector of subgraphs presented by node indexes.
 */
std::vector<std::vector<int>> divide_graph_by_connectivity(const NetworkGraph &graph);


/**
 * @brief Position a subgraph for drawing.
 * @param graph network graph.
 * @param nodes connected subgraph nodes.
 * @param screen_size output window size.
 * @param margin border size for network graph.
 * @param num_iterations number of iterations for the positioning algorithm.
 * @return node positions.
 */
std::vector<cv::Point2i> position_network(
    const NetworkGraph &graph, const std::vector<int> &nodes, cv::Size screen_size, int margin, int num_iterations);


/**
 * @brief Draw a subgraph in process of its positioning.
 * @param graph base network graph.
 * @param nodes connected subgraph nodes.
 * @param screen_size output image size.
 * @param margin size of borders in pixels.
 */
void position_network_test(
    const NetworkGraph &graph, const std::vector<int> &nodes, const cv::Size &screen_size, int margin = 50);
