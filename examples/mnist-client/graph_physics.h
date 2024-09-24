/**
 * @file graph_physics.h
 * @brief Building a graph structure.
 * @date 26.07.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once
#include <vector>

#include <opencv2/core.hpp>


/**
 * @brief A point that has position and velocity.
 */
struct PhysicsPoint
{
    // cppcheck-suppress unusedStructMember
    /**
     * @brief Point index.
     */
    int index_;

    /**
     * @brief Point position.
     */
    cv::Vec2d pos_;

    /**
     * @brief Point velocity.
     */
    cv::Vec2d vel_;
};


/**
 * Graph description with additional parameters used for visualization.
 */
class VisualGraph
{
public:
    /**
     * @brief Construct graph visualizer.
     * @param nodes subgraph node indexes.
     * @param adj_list whole graph adjacency list.
     */
    explicit VisualGraph(const std::vector<int> &nodes, const std::vector<std::vector<size_t>> &adj_list);

    /**
     * @brief Calculate forces, modify point velocities and move points around.
     */
    void iterate();

    /**
     * @brief Iterate N times.
     * @param n number of iterations.
     */
    void iterate(int n);

    /**
     * @brief Fit graph to image size, get point coordinates for the image.
     * @param screen_size output rectangle size.
     * @param margin margin sizes.
     */
    [[nodiscard]] std::vector<cv::Point2i> scale_graph(const cv::Size &screen_size, int margin) const;

    /**
     * @brief Get points positions.
     * @return points positions.
     */
    [[nodiscard]] std::vector<cv::Point2d> get_positions() const;

private:
    // Data
    std::vector<std::vector<size_t>> base_graph_;
    // Edges (true = has an edge).
    std::vector<std::vector<bool>> edges_mat_;
    // Positions and velocities of the nodes.
    std::vector<PhysicsPoint> points_;

    // Hyperparameters
    double spring_strength_ = 1.0f;
    double spring_len_ = 1.0f;
    double repel_coeff_ = 0.3f;
    double resistance_ = 1.0f;

    // Calculates force from one point to another. Force consists of "repel", "spring" and "resistance" components.
    [[nodiscard]] cv::Vec2d get_force(const PhysicsPoint &target, const PhysicsPoint &influence, bool has_edge) const;
};
