/**
 * @file graph_physics.cpp
 * @brief Implement graph physics.
 * @date 21.08.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include "graph_physics.h"

#include <algorithm>
#include <limits>
#include <utility>

#include <opencv2/core.hpp>


// Construct graph class from an adjacency list.
VisualGraph::VisualGraph(const std::vector<int> &nodes, const std::vector<std::vector<size_t>> &adj_list)
    : base_graph_(adj_list)
{
    edges_mat_.resize(base_graph_.size());
    for (auto &edge : edges_mat_) edge.resize(base_graph_.size());

    for (size_t i = 0; i < nodes.size(); ++i)
    {
        PhysicsPoint point{nodes[i], {cv::randu<double>(), cv::randu<double>()}, {0.0, 0.0}};
        points_.push_back(std::move(point));
        for (auto n : adj_list[i]) edges_mat_[i][n] = true;
    }
}


// We show graph using a physical model: all nodes repel each other, all connected nodes are connected as if by
// springs of a fixed length. There is also a drag force to dampen oscillations, it's calculated independently.
// This function calculates forces between two nodes.
cv::Vec2d VisualGraph::get_force(const PhysicsPoint &target, const PhysicsPoint &influence, bool has_edge) const
{
    double very_small_number = 1e-5;
    cv::Vec2d dist = target.pos_ - influence.pos_;
    double len = cv::norm(dist);
    cv::Vec2d force = 0;
    if (has_edge) force += -dist * (len - spring_len_) / std::max(len, very_small_number) * spring_strength_;
    force += dist / (len * len * len + very_small_number) * repel_coeff_;
    return force;
}


// A single iteration recalculates forces and velocities then moves the points according to their corresponding
// velocities.
void VisualGraph::iterate()
{
    double dt = 0.2;
    // Calculate forces
    std::vector<cv::Vec2d> forces;
    forces.resize(points_.size(), {0.0, 0.0});
    for (size_t i = 0; i < points_.size(); ++i)
    {
        for (size_t j = 0; j < points_.size(); ++j)
        {
            if (i == j) continue;
            auto force = get_force(points_[i], points_[j], edges_mat_[i][j] || edges_mat_[j][i]);
            forces[i] += force;
        }
        // Drag
        forces[i] += -points_[i].vel_ * resistance_;
    }
    // Update velocities and positions.
    for (size_t i = 0; i < points_.size(); ++i)
    {
        points_[i].vel_ += forces[i] * dt;
        points_[i].pos_ += points_[i].vel_ * dt;
    }
}


// Iterate N times.
void VisualGraph::iterate(int n)
{
    for (int i = 0; i < n; ++i) iterate();
}


// Get all points positions.
[[nodiscard]] std::vector<cv::Point2d> VisualGraph::get_positions() const
{
    std::vector<cv::Point2d> result;
    result.reserve(points_.size());
    std::transform(
        points_.begin(), points_.end(), std::back_inserter(result),
        [](const auto &v) { return cv::Point2d(v.pos_[0], v.pos_[1]); });
    return result;
}


// Rescale a graph to make it fit inside a rectangle.
std::vector<cv::Point2i> VisualGraph::scale_graph(const cv::Size &screen_size, int margin) const
{
    const double small_num = 1e-5;
    cv::Point2d low_point{std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
    cv::Point2d high_point{std::numeric_limits<double>::min(), std::numeric_limits<double>::min()};
    cv::Size graph_max_size = screen_size - cv::Size{2 * margin, 2 * margin};
    auto points = get_positions();

    // Maybe do a rotation here later, probably using cv::minAreaRect.
    for (const auto &point : points)
    {
        low_point.x = std::min(low_point.x, point.x);
        low_point.y = std::min(low_point.y, point.y);
        high_point.x = std::max(high_point.x, point.x);
        high_point.y = std::max(high_point.y, point.y);
    }

    cv::Size2d actual_size(cv::Point2d{high_point.x - low_point.x, high_point.y - low_point.y});
    double scale_x = static_cast<double>(graph_max_size.width) / (actual_size.width + small_num);
    double scale_y = static_cast<double>(graph_max_size.height) / (actual_size.height + small_num);

    // Rescale to fit dimensions.
    double scale = std::min(scale_x, scale_y);
    std::vector<cv::Point2i> result;
    result.reserve(points.size());
    for (const auto &point : points)
    {
        cv::Point2l pt(
            std::lround((point.x - low_point.x) * scale) + margin,
            std::lround((point.y - low_point.y) * scale) + margin);
        result.push_back(std::move(pt));
    }
    return result;
}
