/**
 * Model execution testing.
 */

#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/framework/model_executor.h>
#include <knp/framework/network.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <generators.h>
#include <tests_common.h>

#include <filesystem>


using Population = knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::PopulationVariants;
using Projection = knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::ProjectionVariants;


TEST(FrameworkSuite, ModelExecutorLoad)
{
    namespace kt = knp::testing;

    kt::BLIFATPopulation population{kt::neuron_generator, 1};
    Projection loop_projection =
        kt::DeltaProjection{population.get_uid(), population.get_uid(), kt::synapse_generator, 1};
    Projection input_projection =
        kt::DeltaProjection{knp::core::UID{false}, population.get_uid(), kt::input_projection_gen, 1};

    knp::framework::Network network;
    network.add_population(std::move(population));
    network.add_projection(std::move(input_projection));
    network.add_projection(std::move(loop_projection));

    //    knp::core::UID input_uid = std::visit([](const auto &proj) { return proj.get_uid(); }, input_projection);
    //    knp::core::UID in_channel_uid;
    //    knp::core::UID out_channel_uid;

    knp::framework::Model model(std::move(network));

    //    model.add_input_channel();

    //    backend.init();
    //    auto endpoint = backend.message_bus_.create_endpoint();

    //    // Create input and output
    //    backend.subscribe<knp::core::messaging::SpikeMessage>(input_uid, {in_channel_uid});
    //    endpoint.subscribe<knp::core::messaging::SpikeMessage>(out_channel_uid, {population.get_uid()});

    //    knp::framework::ModelExecutor me(knp::testing::get_backend_path());

    //    std::vector<size_t> results;

    //    for (size_t step = 0; step < 20; ++step)
    //    {
    //        // Send inputs on steps 0, 5, 10, 15
    //        if (step % 5 == 0)
    //        {
    //            knp::core::messaging::SpikeMessage message{{in_channel_uid, 0}, {0}};
    //            endpoint.send_message(message);
    //        }
    //        backend.step();
    //        endpoint.receive_all_messages();
    //        auto output = endpoint.unload_messages<knp::core::messaging::SpikeMessage>(out_channel_uid);
    //        // Write up the steps where the network sends a spike
    //        if (!output.empty()) results.push_back(step);
    //    }

    //    // Spikes on steps "5n + 1" (input) and on "previous_spike_n + 6" (positive feedback loop)
    //    const std::vector<size_t> expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};
    //    ASSERT_EQ(results, expected_results);
}
