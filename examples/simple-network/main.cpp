/**
 * @file main.cpp
 * @brief Example of running a simple network.
 * @author A. Vartenkov
 * @date 28.03.2024
 */

#include <knp/framework/io/out_converters/convert_set.h>
#include <knp/framework/model_executor.h>
#include <knp/framework/network.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <filesystem>


using DeltaProjection = knp::core::Projection<knp::synapse_traits::DeltaSynapse>;
using BLIFATPopulation = knp::core::Population<knp::neuron_traits::BLIFATNeuron>;

// Create an input projection
inline std::optional<DeltaProjection::Synapse> input_projection_gen(size_t /*index*/)  // NOLINT
{
    return DeltaProjection::Synapse{{1.0, 1, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};
}


// Create a loop projection
inline std::optional<DeltaProjection::Synapse> synapse_generator(size_t /*index*/)  // NOLINT
{
    return DeltaProjection::Synapse{{1.0, 6, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};
}

// Create population
inline knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron> neuron_generator(size_t)  // NOLINT
{
    return knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>{};
}


int main(int argc, const char *const argv[])
{
    // Create new populations.
    BLIFATPopulation population{neuron_generator, 1};
    DeltaProjection loop_projection = DeltaProjection{population.get_uid(), population.get_uid(), synapse_generator, 1};
    DeltaProjection input_projection =
        DeltaProjection{knp::core::UID{false}, population.get_uid(), input_projection_gen, 1};

    knp::core::UID input_uid = input_projection.get_uid();
    knp::core::UID output_uid = population.get_uid();

    knp::framework::Network network;

    // Add populations to the network.
    network.add_population(std::move(population));
    network.add_projection(std::move(input_projection));
    network.add_projection(std::move(loop_projection));

    knp::core::UID i_channel_uid, o_channel_uid;

    knp::framework::Model model(std::move(network));
    model.add_input_channel(i_channel_uid, input_uid);
    model.add_output_channel(o_channel_uid, output_uid);

    auto input_gen = [](knp::core::Step step) -> knp::core::messaging::SpikeData
    {
        if (step % 5 == 0)
        {
            knp::core::messaging::SpikeData s;
            s.push_back(0);
            return s;
        }
        return knp::core::messaging::SpikeData();
    };

    auto backend_path = std::filesystem::path(argv[0]).parent_path() / "knp-cpu-single-threaded-backend";
    knp::framework::BackendLoader backend_loader;

    // Load backend and create model executor.
    knp::framework::ModelExecutor model_executor(
        model, backend_loader.load(backend_path), {{i_channel_uid, input_gen}});

    auto &out_channel = model_executor.get_loader().get_output_channel(o_channel_uid);

    // Run model.
    model_executor.start([](size_t step) { return step < 20; });

    std::vector<knp::core::Step> results;
    // Get model results.
    const auto &spikes = out_channel.update();
    results.reserve(spikes.size());

    std::transform(
        spikes.cbegin(), spikes.cend(), std::back_inserter(results),
        [](const auto &spike_msg) { return spike_msg.header_.send_time_; });
    // Spikes on steps "5n + 1" (input) and on "previous_spike_n + 6" (positive
    // feedback loop) const std::vector<knp::core::messaging::Step>
    // expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};
    for (const auto &s : results) std::cout << s << " ";
    std::cout << std::endl;
}
