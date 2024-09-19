//
// Created by an_vartenkov on 18.09.24.
//
#include "inference.h"

#include <knp/framework/io/input_channel.h>
#include <knp/framework/model.h>
#include <knp/framework/model_executor.h>
#include <knp/framework/monitoring/observer.h>
#include <knp/framework/network.h>

#include <unordered_map>
#include <utility>
#include <vector>

#include "load_from_csv.h"
#include "process_data.h"


auto make_input_image_generator(const std::vector<std::vector<bool>> &spike_frames, size_t skip = 0)
{
    auto generator = [&spike_frames](knp::core::Step step)
    {
        knp::core::messaging::SpikeData message;

        if (step >= spike_frames.size()) return message;

        for (size_t i = 0; i < spike_frames[step].size(); ++i)
        {
            if (spike_frames[step][i]) message.push_back(i);
        }
        return message;
    };

    return generator;
}


std::vector<InferenceResult> do_inference(
    const std::filesystem::path &path_to_model, const std::filesystem::path &path_to_data,
    const std::filesystem::path &path_to_backend, int tact, int last_step)
{
    knp::core::UID output_uid;
    std::vector<std::pair<knp::core::UID, size_t>> input_uids;

    knp::framework::Network network = create_network_from_monitoring_file(path_to_model, tact, {}, input_uids);

    // Make a list of all populations and their sizes
    std::unordered_map<knp::core::UID, size_t, knp::core::uid_hash> population_sizes;
    for (const auto &pop : network.get_populations())
    {
        knp::core::UID uid = std::visit([](const auto &p) { return p.get_uid(); }, pop);
        size_t size = std::visit([](const auto &p) { return p.size(); }, pop);
        population_sizes.insert({uid, size});
    }

    knp::framework::Model model(std::move(network));

    if (input_uids.empty()) throw std::runtime_error("Wrong model file: model doesn't have inputs");

    std::vector<knp::core::UID> input_image_projection_uids;

    // The largest projection is the input image projection.
    constexpr size_t img_input_size = 117600;
    constexpr size_t output_population_size = 10;

    for (auto v : input_uids)
    {
        if (v.second == img_input_size)
        {
            input_image_projection_uids.push_back(v.first);
        }
    }

    std::vector<knp::core::UID> input_image_channel_uids(input_image_projection_uids.size());
    for (size_t i = 0; i < input_image_projection_uids.size(); ++i)
    {
        model.add_input_channel(input_image_channel_uids[i], input_image_projection_uids[i]);
    }

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
    if (last_step < 0) last_step = spike_frames.size();

    knp::framework::ModelLoader::InputChannelMap channel_map;

    for (auto img_channel_uid : input_image_channel_uids)
        channel_map.insert({img_channel_uid, make_input_image_generator(spike_frames)});

    knp::framework::BackendLoader backend_loader;
    knp::framework::ModelExecutor model_executor(model, backend_loader.load(path_to_backend), std::move(channel_map));
    std::vector<InferenceResult> result;

    std::vector<knp::core::UID> output_populations;
    for (const auto &pop : model.get_network().get_populations())
    {
        if (std::visit([](const auto &p) { return p.size(); }, pop) == output_population_size)
            output_populations.push_back(std::visit([](const auto &p) { return p.get_uid(); }, pop));
    }

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

    model_executor.add_observer<knp::core::messaging::SpikeMessage>(observer_func, output_populations);
    model_executor.start(
        [last_step](size_t step)
        {
            if (step % 20 == 0) std::cout << "Step: " << step << std::endl;
            return step != last_step;
        });
    return result;
}
