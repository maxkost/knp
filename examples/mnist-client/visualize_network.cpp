//
// Created by an_vartenkov on 11.07.24.
//

#include "visualize_network.h"

#include <knp/framework/network.h>

#include <algorithm>

#include <nodesoup.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


NetworkGraph::NetworkGraph(const knp::framework::Network &network)
{
    for (const auto &pop : network.get_populations())
    {
        size_t pop_size = std::visit([](const auto &p) { return p.size(); }, pop);
        knp::core::UID uid = std::visit([](const auto &p) { return p.get_uid(); }, pop);
        nodes_.push_back(Node{pop_size, uid, pop.index()});
    }

    for (const auto &proj : network.get_projections())
    {
        size_t proj_size = std::visit([](const auto &p) { return p.size(); }, proj);
        knp::core::UID uid = std::visit([](const auto &p) { return p.get_uid(); }, proj);
        knp::core::UID uid_from = std::visit([](const auto &p) { return p.get_presynaptic(); }, proj);
        knp::core::UID uid_to = std::visit([](const auto &p) { return p.get_postsynaptic(); }, proj);
        int id_from = -1, id_to = -1;
        for (size_t i = 0; i < nodes_.size(); ++i)
        {
            if (uid_from == nodes_[i].uid_) id_from = i;
            if (uid_to == nodes_[i].uid_) id_to = i;
        }
        edges_.push_back(Edge{proj_size, id_from, id_to, uid, proj.index()});
    }
}


/**
 * Converts network graph into adjacency list form.
 * @param graph
 * @return graph as an adjacency list (see https://en.wikipedia.org/wiki/Adjacency_list).
 * @note There is no input node in NetworkGraph, but an adjacency list must have one, it's the last node.
 */
nodesoup::adj_list_t build_adjacency_list(const NetworkGraph &graph)
{
    nodesoup::adj_list_t adj_list;
    adj_list.resize(graph.nodes_.size() + 1);
    for (const auto &edge : graph.edges_)
    {
        int index = edge.index_from_;
        if (index < 0) index = graph.nodes_.size();
        adj_list[index].push_back(edge.index_to_);
    }
    return adj_list;
}


/*
 * @brief Draws a line with an arrow that is much nicer than basic OpenCV arrow.
 * @param img image to be modified.
 * @param pt_from point to start the arrow from.
 * @param pt_to the point arrow points to.
 * @param len arrow length in pixels.
 * @param width arrow width as a fraction of arrow length.
 * @param margin shifts the arrow from the end of the line to the beginning by "margin" pixels.
 * @param color the color of both line and arrow.
 */
void draw_simple_arrow_line(
    cv::Mat &img, const cv::Point2d &pt_from, const cv::Point2d &pt_to, int len, double width, int margin,
    const cv::Scalar &color)
{
    cv::line(img, pt_from, pt_to, color);
    cv::Point2d direction = pt_to - pt_from;
    direction /= cv::norm(direction);
    cv::Point2d op_direction{-direction.y, direction.x};

    cv::Point2d pt_arrow_end = pt_to - direction * margin;
    cv::Point2d pt_arrow_back = pt_to - direction * (len + margin);
    cv::Point2d pt_back_pos_1 = pt_arrow_back + width * len * op_direction;
    cv::Point2d pt_back_pos_2 = pt_arrow_back - width * len * op_direction;
    cv::fillConvexPoly(img, std::vector<cv::Point2i>{pt_back_pos_1, pt_arrow_end, pt_back_pos_2}, color);
}


// TODO : draw graph of selected nodes, while all nodes not included there are considered external.
cv::Mat draw_subgraph(
    const nodesoup::adj_list_t &adj_list, const nodesoup::adj_list_t &rev_list, const std::vector<int> &nodes,
    const std::vector<cv::Point2i> &points, const cv::Size &img_size)
{
    return {};
}


/*
 * @brief Draw connected subgraph. It shouldn't have any connections to the other parts of the graph, excluding inputs.
 * @param adj_list adjacency list of the whole graph.
 * @param nodes nodes that are part of the subgraph
 * @param points nodes coordinates.
 * @param inputs input nodes. The function draws a vertical black arrow if a node is connected to input.
 * @param img_size output image size.
 * @return image of a drawn subgraph.
 */
cv::Mat draw_subgraph(
    const nodesoup::adj_list_t &adj_list, const std::vector<int> &nodes, const std::vector<cv::Point2i> &points,
    const std::vector<int> &inputs, const cv::Size &img_size)
{
    const cv::Scalar node_color{0, 0, 0};
    const cv::Scalar back_color{255, 255, 255};
    const cv::Scalar edge_color{0, 0, 255};
    const int self_arrow_radius = 30;
    const int node_radius = 10;
    const int arrow_len = 20;
    const double arrow_width = 0.3;

    cv::Mat out_img{img_size, CV_8UC3, back_color};

    // Draw edges
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        // This is a bit nonobvious. Adj_list is a list for all nodes in the graph,
        // while i is the position of node in subgraph.
        // position of node in the whole graph is nodes[i].
        for (size_t j = 0; j < adj_list[nodes[i]].size(); ++j)
        {
            // index_from = i
            auto target_node_iter = std::find(nodes.begin(), nodes.end(), adj_list[nodes[i]][j]);
            if (target_node_iter == nodes.end()) continue;

            size_t target_point_index = target_node_iter - nodes.begin();
            if (target_point_index == i)
            {
                cv::circle(out_img, points[i] - cv::Point2i(0, self_arrow_radius), self_arrow_radius, edge_color);
                continue;
            }
            const cv::Point2i &point_from = points[i];
            const cv::Point2i &point_to = points[target_point_index];
            draw_simple_arrow_line(out_img, point_from, point_to, arrow_len, arrow_width, node_radius, edge_color);
        }
    }

    // Draw inputs (black arrow from above)
    for (auto input : inputs)
    {
        auto target_node_iter = std::find(nodes.begin(), nodes.end(), input);
        if (target_node_iter == nodes.end()) continue;
        cv::Point2i point = points[target_node_iter - nodes.begin()];
        draw_simple_arrow_line(
            out_img, point - cv::Point2i(0, 2 * arrow_len + node_radius), point, arrow_len, arrow_width, node_radius,
            node_color);
    }

    // Draw nodes
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        // Drawing
        const auto &point = points[i];
        cv::circle(out_img, {static_cast<int>(point.x), static_cast<int>(point.y)}, node_radius, node_color, -1);
    }
    return out_img;
}


// Make a reverse adjacent list. It's used to quickly find "incoming" nodes.
nodesoup::adj_list_t make_reverse_list(const nodesoup::adj_list_t &adj_list)
{
    nodesoup::adj_list_t rev_list;
    rev_list.resize(adj_list.size());
    for (size_t count = 0; count < adj_list.size(); ++count)
    {
        for (auto val : adj_list[count]) rev_list[val].push_back(count);
    }
    return rev_list;
}


// Prints nodes, connections and population/projection sizes. Not very readable.
void print_modified_network_description(const NetworkGraph &graph)
{
    auto adjacency_list = build_adjacency_list(graph);
    std::vector<std::vector<int>> outgoing;
    outgoing.resize(adjacency_list.size());
    for (size_t count = 0; count < adjacency_list.size(); ++count)
    {
        for (auto val : adjacency_list[count]) outgoing[val].push_back(count);
    }

    for (size_t i = 0; i < adjacency_list.size(); ++i)
    {
        std::cout << i << ":\nin:";
        for (auto v : outgoing[i]) std::cout << " " << v;
        std::cout << std::endl << "out:";
        for (auto v : adjacency_list[i]) std::cout << " " << v;
        std::cout << std::endl;
    }
}


// Finds an independent subgraph inside a larger graph.
std::vector<int> find_connected_set(
    const nodesoup::adj_list_t &adj_list, const nodesoup::adj_list_t &rev_list,
    std::unordered_set<int> &remaining_nodes, const std::unordered_set<int> &ignore_nodes = {})
{
    int node = *remaining_nodes.begin();
    std::unordered_set<int> processed_nodes;
    std::unordered_set<int> nonproc_nodes{node};
    while (!nonproc_nodes.empty())
    {
        int curr_node = *nonproc_nodes.begin();
        const auto &out_nodes = adj_list[curr_node];
        for (auto node_out : out_nodes)
        {
            int n = static_cast<int>(node_out);
            if (n == curr_node) continue;
            if (processed_nodes.find(n) != processed_nodes.end()) continue;
            if (ignore_nodes.find(n) != ignore_nodes.end())
            {
                processed_nodes.insert(n);
                continue;
            }
            nonproc_nodes.insert(n);
        }
        const auto &rev_nodes = rev_list[curr_node];
        for (auto node_out : rev_nodes)
        {
            int n = static_cast<int>(node_out);
            if (n == curr_node) continue;
            if (processed_nodes.find(n) != processed_nodes.end()) continue;
            if (ignore_nodes.find(n) != ignore_nodes.end())
            {
                // processed_nodes.insert(n);
                continue;
            }
            nonproc_nodes.insert(n);
        }
        nonproc_nodes.erase(curr_node);
        processed_nodes.insert(curr_node);
    }
    for (int node : processed_nodes) remaining_nodes.erase(node);
    // remaining_nodes.erase(processed_nodes.begin(), processed_nodes.end());
    std::vector<int> result;
    result.resize(processed_nodes.size());
    std::copy(processed_nodes.begin(), processed_nodes.end(), result.begin());
    std::sort(result.begin(), result.end());
    return result;
}


// Find all independent components inside a graph.
std::vector<std::vector<int>> divide_graph_by_connectivity(const NetworkGraph &graph)
{
    nodesoup::adj_list_t adj_list = build_adjacency_list(graph);
    nodesoup::adj_list_t rev_list = make_reverse_list(adj_list);
    std::unordered_set<int> remaining_nodes;
    std::vector<std::vector<int>> connected_sets;
    std::unordered_set<int> ignored_nodes{static_cast<int>(adj_list.size() - 1)};
    for (int i = 0; i < adj_list.size(); ++i) remaining_nodes.insert(i);

    remaining_nodes.erase(adj_list.size() - 1);  // delete input node
    while (!remaining_nodes.empty())
    {
        connected_sets.push_back(find_connected_set(adj_list, rev_list, remaining_nodes, ignored_nodes));
    }

    return connected_sets;
}


// Prints a subset.
void print_connected_subset(
    const NetworkGraph &graph, const nodesoup::adj_list_t &adj_list, const nodesoup::adj_list_t &rev_list,
    const std::vector<int> &nodes)
{
    for (auto node : nodes)
    {
        std::cout << "Population #" << node << " of size " << graph.nodes_[node].size_ << ": receive from";
        for (auto node_from : rev_list[node])
            if (node_from == adj_list.size() - 1)
                std::cout << " #INPUT";
            else
                std::cout << " #" << node_from;
        std::cout << "; send to";
        for (auto node_to : adj_list[node]) std::cout << " #" << node_to;
        std::cout << std::endl;
    }
}


// Prints all subsets.
void print_network_description(const NetworkGraph &graph)
{
    nodesoup::adj_list_t adj_list = build_adjacency_list(graph);
    nodesoup::adj_list_t rev_list = make_reverse_list(adj_list);
    auto connected_subsets = divide_graph_by_connectivity(graph);
    for (const auto &subset : connected_subsets)
    {
        print_connected_subset(graph, adj_list, rev_list, subset);
        std::cout << std::endl;
    }
}


// Graph drawing
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
    explicit VisualGraph(const std::vector<int> &nodes, const nodesoup::adj_list_t &adj_list);

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
    nodesoup::adj_list_t base_graph_;
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


VisualGraph::VisualGraph(const std::vector<int> &nodes, const nodesoup::adj_list_t &adj_list) : base_graph_(adj_list)
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
    for (const auto &point : points_) result.push_back(cv::Point2d(point.pos_[0], point.pos_[1]));
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


// Get node positions from Network Graph
std::vector<cv::Point2i> position_network(
    const NetworkGraph &graph, const std::vector<int> &nodes, const cv::Size &screen_size, int margin,
    int num_iterations)
{
    VisualGraph vis_graph(nodes, build_adjacency_list(graph));
    vis_graph.iterate(num_iterations);
    return vis_graph.scale_graph(screen_size, margin);
}


// Draw a subgraph
void position_network_test(
    const NetworkGraph &graph, const std::vector<int> &nodes, const cv::Size &screen_size, int margin)
{
    cv::theRNG().state = std::time(nullptr);
    nodesoup::adj_list_t adj_list = build_adjacency_list(graph);
    VisualGraph vis_graph(nodes, adj_list);
    int key = 0;

    // Create inputs
    std::vector<int> inputs;
    inputs.reserve(adj_list.back().size());
    for (auto v : adj_list.back()) inputs.push_back(static_cast<int>(v));

    while (key != 27)
    {
        auto points = vis_graph.scale_graph(screen_size, margin);
        cv::Mat img = draw_subgraph(adj_list, nodes, points, inputs, screen_size);
        cv::imshow("Graph", img);
        key = cv::waitKey(50) & 255;
        vis_graph.iterate(1);
    }
}
