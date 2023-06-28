/**
 * Created by Vartenkov An. on 07.04.23.
 */

#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <spdlog/spdlog.h>
#include <tests_common.h>

#include <vector>


using DeltaProjection = knp::core::Projection<knp::synapse_traits::DeltaSynapse>;
using BLIFATPopulation = knp::core::Population<knp::neuron_traits::BLIFATNeuron>;
using Population = knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::PopulationVariants;
using Projection = knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::ProjectionVariants;


// Create an input projection
DeltaProjection::SynapseGenerator input_projection_gen = [](size_t index) -> std::optional<DeltaProjection::Synapse> {
    return DeltaProjection::Synapse{{1.0, 1, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};
};


// Create a loop projection
DeltaProjection::SynapseGenerator synapse_generator = [](size_t index) -> std::optional<DeltaProjection::Synapse> {
    return DeltaProjection::Synapse{{1.0, 6, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};
};


// Create population
auto neuron_generator = [](size_t index)
{ return knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>{}; };


class TestingBack : public knp::backends::single_threaded_cpu::SingleThreadedCPUBackend
{
public:
    TestingBack() = default;
    void init() override { knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::init(); }
};


TEST(SingleThreadCpuSuite, SmallestNetwork)
{
    // Create a single neuron network: input -> input_projection -> population <=> loop_projection
    TestingBack backend;

    BLIFATPopulation population{neuron_generator, 1};
    Projection loop_projection = DeltaProjection{population.get_uid(), population.get_uid(), synapse_generator, 1};
    Projection input_projection = DeltaProjection{knp::core::UID{false}, population.get_uid(), input_projection_gen, 1};
    knp::core::UID input_uid = std::visit([](const auto &proj) { return proj.get_uid(); }, input_projection);

    backend.load_populations({population});
    backend.load_projections({input_projection, loop_projection});

    std::cout << "AAAA: " << backend.get_supported_projection_indexes().size() << std::endl;

    backend.init();
    auto endpoint = backend.message_bus_.create_endpoint();

    knp::core::UID in_channel_uid;
    knp::core::UID out_channel_uid;

    // Create input and output
    backend.subscribe<knp::core::messaging::SpikeMessage>(input_uid, {in_channel_uid});
    endpoint.subscribe<knp::core::messaging::SpikeMessage>(out_channel_uid, {population.get_uid()});

    std::vector<size_t> results;

    for (size_t step = 0; step < 20; ++step)
    {
        // Send inputs on steps 0, 5, 10, 15
        if (step % 5 == 0)
        {
            knp::core::messaging::SpikeMessage message{{in_channel_uid, 0}, {0}};
            endpoint.send_message(message);
        }
        backend.step();
        endpoint.receive_all_messages();
        auto output = endpoint.unload_messages<knp::core::messaging::SpikeMessage>(out_channel_uid);
        // Write up the steps where the network sends a spike
        if (!output.empty()) results.push_back(step);
    }

    // Spikes on steps "5n + 1" (input) and on "previous_spike_n + 6" (positive feedback loop)
    const std::vector<size_t> expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};
    ASSERT_EQ(results, expected_results);
}


TEST(SingleThreadCpuSuite, NeuronsGettingTest)
{
    TestingBack backend;

    auto s_neurons = backend.get_supported_neurons();

    ASSERT_GE(s_neurons.size(), 1);
    ASSERT_EQ(s_neurons[0], "knp::neuron_traits::BLIFATNeuron");
}


TEST(SingleThreadCpuSuite, SynapsesGettingTest)
{
    TestingBack backend;

    auto s_synapses = backend.get_supported_synapses();

    ASSERT_GE(s_synapses.size(), 1);
    ASSERT_EQ(s_synapses[0], "knp::synapse_traits::DeltaSynapse");
}
