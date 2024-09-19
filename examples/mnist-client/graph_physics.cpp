/**
 * @file graph_physics.cpp
 * @brief Implement graph physics.
 * @author A. Vartenkov
 * @date 21.08.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include "graph_physics.h"

#include <algorithm>
#include <limits>
#include <utility>

#include <opencv2/core.hpp>


VisualGraph::VisualGraph(const std::vector<int> &nodes, const AdjacencyList &adj_list) : base_graph_(adj_list)
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


cv::Vec2d VisualGraph::get_force(const PhysicsPoint &target, const PhysicsPoint &influence, bool has_edge) const
{
    double very_small_number = 1e-5;
    cv::Vec2d dist = target.pos_ - influence.pos_;
    double len = cv::norm(dist);
    cv::Vec2d force = 0;
    if (has_edge) force += -dist * (len - spring_len) / std::max(len, very_small_number) * spring_strength;
    force += dist / (len * len * len + very_small_number) * repel_coeff;
    return force;
}


void VisualGraph::iterate()
{
    double dt = 0.2;
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
        forces[i] += -points_[i].vel_ * resistance_;
    }
    for (size_t i = 0; i < points_.size(); ++i)
    {
        points_[i].vel_ += forces[i] * dt;
        points_[i].pos_ += points_[i].vel_ * dt;
    }
}


void VisualGraph::iterate(int n)
{
    for (int i = 0; i < n; ++i) iterate();
}


[[nodiscard]] std::vector<cv::Point2d> VisualGraph::get_positions() const
{
    std::vector<cv::Point2d> result;
    result.reserve(points_.size());
    // for (const auto &point : points_) result.push_back(cv::Point2d(point.pos_[0], point.pos_[1]));
    std::transform(
        points_.begin(), points_.end(), std::back_inserter(result),
        [](const auto &v) { return cv::Point2d(v.pos_[0], v.pos_[1]); });
    return result;
}


std::vector<cv::Point2i> VisualGraph::scale_graph(const cv::Size &screen_size, int margin) const
{
    const double small_num = 1e-5;
    cv::Point2d low_point{std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
    cv::Point2d high_point{std::numeric_limits<double>::min(), std::numeric_limits<double>::min()};
    cv::Size graph_max_size = screen_size - cv::Size{2 * margin, 2 * margin};
    auto points = get_positions();
    for (const auto &point : points)
    {
        low_point.x = std::min(low_point.x, point.x);
        low_point.y = std::min(low_point.y, point.y);
        high_point.x = std::max(high_point.x, point.x);
        high_point.y = std::max(high_point.y, point.y);
    }
    cv::Size2d actual_size(cv::Point2d{high_point.x - low_point.x, high_point.y - low_point.y});
    double scale_x = 1.0 * graph_max_size.width / (actual_size.width + small_num);
    double scale_y = 1.0 * graph_max_size.height / (actual_size.height + small_num);
    // maybe do a rotation here later

    // rescale to fit dimensions

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
