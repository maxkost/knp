/**
 * @file inference.cpp
 * @brief Do inference for MNIST example network.
 * @date 18.09.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include "inference.h"

#include <knp/framework/io/input_channel.h>
#include <knp/framework/model.h>
#include <knp/framework/model_executor.h>
#include <knp/framework/monitoring/observer.h>
#include <knp/framework/network.h>
#include <knp/framework/sonata/network_io.h>

#include <unordered_map>
#include <utility>
#include <vector>

#include "process_data.h"


// Create a spike message generator from an array of boolean frames.
auto make_input_image_generator(const std::vector<std::vector<bool>> &spike_frames, size_t skip = 0)
{
    auto generator = [&spike_frames, skip](knp::core::Step step)
    {
        knp::core::messaging::SpikeData message;

        if (step >= spike_frames.size()) return message;

        for (size_t i = 0; i < spike_frames[step + skip].size(); ++i)
        {
            if (spike_frames[step + skip][i]) message.push_back(i);
        }
        return message;
    };

    return generator;
}


std::vector<knp::core::UID> find_inputs(
    const knp::framework::Network &network,
    const std::function<bool(const knp::core::AllProjectionsVariant &)> &is_input)
{
    std::vector<knp::core::UID> uids;
    for (const auto &proj : network.get_projections())
    {
        if (is_input(proj)) uids.push_back(std::visit([](const auto &p) { return p.get_uid(); }, proj));
    }
    return uids;
}


// Do MNIST inference using binary data file and csv model. See function description in inference.h.
std::vector<InferenceResult> do_inference(
    const std::filesystem::path &path_to_model, const std::filesystem::path &path_to_data,
    const std::filesystem::path &path_to_backend, int last_step)
{
    std::vector<std::pair<knp::core::UID, size_t>> input_uids;
    knp::framework::Network network = knp::framework::sonata::load_network(path_to_model);

    // Make a list of all populations and their sizes
    std::unordered_map<knp::core::UID, size_t, knp::core::uid_hash> population_sizes;
    for (const auto &pop : network.get_populations())
    {
        knp::core::UID uid = std::visit([](const auto &p) { return p.get_uid(); }, pop);
        size_t size = std::visit([](const auto &p) { return p.size(); }, pop);
        population_sizes.insert({uid, size});
    }

    knp::framework::Model model(std::move(network));

    // The largest projection is the input image projection. These are numbers for a specific MNIST model.
    constexpr size_t img_input_size = 117600;
    constexpr size_t output_population_size = 10;

    auto is_input = [](const knp::core::AllProjectionsVariant &proj)
    { return std::visit([](const auto &p) { return p.size(); }, proj) == img_input_size; };

    std::vector<knp::core::UID> input_image_projection_uids = find_inputs(model.get_network(), is_input);
    if (input_image_projection_uids.empty()) throw std::runtime_error("Wrong model file: model doesn't have inputs");


    // Add input channel for each input projection that accepts an image.
    std::vector<knp::core::UID> input_image_channel_uids(input_image_projection_uids.size());
    for (size_t i = 0; i < input_image_projection_uids.size(); ++i)
    {
        model.add_input_channel(input_image_channel_uids[i], input_image_projection_uids[i]);
    }

    // Image-to-spikes conversion parameters.
    constexpr int intensity_levels = 10;
    constexpr int frames_per_image = 20;
    constexpr size_t input_size = 28 * 28;
    constexpr size_t skip = 0;
    auto spike_frames =
        read_spikes_from_grayscale_file(path_to_data, input_size, frames_per_image, intensity_levels, skip);
    if (last_step > 0 && last_step < spike_frames.size())
    {
        spike_frames.resize(last_step);
        spike_frames.shrink_to_fit();
    }
    else
        last_step = spike_frames.size();

    knp::framework::ModelLoader::InputChannelMap channel_map;

    for (auto img_channel_uid : input_image_channel_uids)
        channel_map.insert({img_channel_uid, make_input_image_generator(spike_frames)});

    knp::framework::BackendLoader backend_loader;
    knp::framework::ModelExecutor model_executor(model, backend_loader.load(path_to_backend), std::move(channel_map));
    std::vector<InferenceResult> result;

    // Find output populations.
    std::vector<knp::core::UID> output_populations;
    for (const auto &pop : model.get_network().get_populations())
    {
        if (std::visit([](const auto &p) { return p.size(); }, pop) == output_population_size)
            output_populations.push_back(std::visit([](const auto &p) { return p.get_uid(); }, pop));
    }

    // Make an observer function that outputs resulting spikes to terminal.
    auto observer_func = [&result](const std::vector<knp::core::messaging::SpikeMessage> &messages)
    {
        if (messages.empty() || messages[0].neuron_indexes_.empty()) return;
        InferenceResult result_buf;
        result_buf.step_ = messages[0].header_.send_time_;
        for (auto index : messages[0].neuron_indexes_)
        {
            std::cout << index << " ";
            result_buf.indexes_.push_back(index);
        }
        result.push_back(result_buf);
        std::cout << std::endl;
    };

    // Add observer.
    model_executor.add_observer<knp::core::messaging::SpikeMessage>(observer_func, output_populations);

    // Start model.
    model_executor.start(
        [last_step](size_t step)
        {
            if (step % 20 == 0) std::cout << "Step: " << step << std::endl;
            return step != last_step;
        });
    return result;
}
