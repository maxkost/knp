/**
 * @file graph_physics.h
 * @brief Building a graph structure.
 * @author A. Vartenkov
 * @date 26.07.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once
#include <vector>

#include <opencv2/core.hpp>


using AdjacencyList = std::vector<std::vector<size_t>>;


struct PhysicsPoint
{
    int index_;
    cv::Vec2d pos_;
    cv::Vec2d vel_;
};


class VisualGraph
{
public:
    // Construct graph visualizer.
    // nodes - subgraph node indexes.
    // adj_list - whole graph adjacency list.
    explicit VisualGraph(const std::vector<int> &nodes, const AdjacencyList &adj_list);

    // Calculate forces, modify point velocities and move points around.
    void iterate();

    // iterate() n times.
    void iterate(int n);

    // Fit graph to image size, get point coordinates for the image.
    [[nodiscard]] std::vector<cv::Point2i> scale_graph(const cv::Size &screen_size, int margin) const;

    // Get point positions
    [[nodiscard]] std::vector<cv::Point2d> get_positions() const;

private:
    // Data
    AdjacencyList base_graph_;
    std::vector<std::vector<bool>> edges_mat_;
    std::vector<PhysicsPoint> points_;

    // Hyperparameters
    double spring_strength = 1.0f;
    double spring_len = 1.0f;
    double repel_coeff = 0.3f;
    double resistance_ = 1.0f;

    // Calculates force from one point to another. Force consists of "repel", "spring" and "resistance" components.
    [[nodiscard]] cv::Vec2d get_force(const PhysicsPoint &target, const PhysicsPoint &influence, bool has_edge) const;
};
