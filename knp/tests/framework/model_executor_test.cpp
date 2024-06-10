/**
 * @file model_executor_test.cpp
 * @brief Model executor class testing.
 * @author Artiom N.
 * @date 13.07.2023
 */

#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/framework/io/out_converters/convert_set.h>
#include <knp/framework/io/type.h>
#include <knp/framework/model_executor.h>
#include <knp/framework/network.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <generators.h>
#include <spdlog/spdlog.h>
#include <tests_common.h>

#include <filesystem>


TEST(FrameworkSuite, ModelExecutorLoad)
{
    namespace kt = knp::testing;

    kt::BLIFATPopulation population{kt::neuron_generator, 1};
    kt::DeltaProjection loop_projection =
        kt::DeltaProjection{population.get_uid(), population.get_uid(), kt::synapse_generator, 1};
    kt::DeltaProjection input_projection =
        kt::DeltaProjection{knp::core::UID{false}, population.get_uid(), kt::input_projection_gen, 1};

    const knp::core::UID input_uid = input_projection.get_uid();
    const knp::core::UID output_uid = population.get_uid();

    knp::framework::Network network;
    network.add_population(std::move(population));
    network.add_projection<kt::DeltaProjection>(std::move(input_projection));
    network.add_projection<kt::DeltaProjection>(std::move(loop_projection));

    const knp::core::UID i_channel_uid, o_channel_uid;

    knp::framework::Model model(std::move(network));
    SPDLOG_DEBUG("Adding input channel {} to projection {}", std::string(i_channel_uid), std::string(input_uid));
    model.add_input_channel(i_channel_uid, input_uid);
    SPDLOG_DEBUG("Adding output channel {} to population {}", std::string(o_channel_uid), std::string(output_uid));
    model.add_output_channel(o_channel_uid, output_uid);

    auto input_gen = [](knp::core::Step step) -> knp::core::messaging::SpikeData
    {
        if (step % 5 == 0)
        {
            knp::core::messaging::SpikeData spike_data;
            spike_data.push_back(0);
            return spike_data;
        }
        return {};
    };

    knp::framework::ModelExecutor model_executor(model, knp::testing::get_backend_path(), {{i_channel_uid, input_gen}});

    auto &out_channel = model_executor.get_output_channel(o_channel_uid);

    model_executor.start([](size_t step) { return step < 20; });

    std::vector<knp::core::Step> results;
    const auto &spikes = out_channel.update();
    results.reserve(spikes.size());

    std::transform(
        spikes.cbegin(), spikes.cend(), std::back_inserter(results),
        [](const auto &spike_msg) { return spike_msg.header_.send_time_; });
    // Spikes on steps "5n + 1" (input) and on "previous_spike_n + 6" (positive feedback loop)
    const std::vector<knp::core::Step> expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};
    ASSERT_EQ(results, expected_results);

    auto pop_tag = std::any_cast<knp::framework::io::IOType>(model.get_network()
                                                                 .get_population<kt::BLIFATPopulation>(output_uid)
                                                                 .get_tags()[knp::framework::io::io_type_tag]);
    auto proj_tag = std::any_cast<knp::framework::io::IOType>(
        model.get_network().get_projection<kt::DeltaProjection>(input_uid).get_tags()[knp::framework::io::io_type_tag]);
    ASSERT_EQ(pop_tag, knp::framework::io::IOType::output);
    ASSERT_EQ(proj_tag, knp::framework::io::IOType::input);
}
