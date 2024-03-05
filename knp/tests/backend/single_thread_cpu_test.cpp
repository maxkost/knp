/**
 * @brief Single threading backend test.
 * @author Vartenkov An.
 * @date 07.04.23.
 */

#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <generators.h>
#include <spdlog/spdlog.h>
#include <tests_common.h>

#include <vector>


using Population = knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::PopulationVariants;
using Projection = knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::ProjectionVariants;


namespace knp::testing
{

class STestingBack : public knp::backends::single_threaded_cpu::SingleThreadedCPUBackend
{
public:
    STestingBack() = default;
    void _init() override { knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::_init(); }
};

}  // namespace knp::testing


TEST(SingleThreadCpuSuite, SmallestNetwork)
{
    // Create a single neuron network: input -> input_projection -> population <=> loop_projection
    knp::testing::STestingBack backend;

    knp::testing::BLIFATPopulation population{knp::testing::neuron_generator, 1};
    Projection loop_projection =
        knp::testing::DeltaProjection{population.get_uid(), population.get_uid(), knp::testing::synapse_generator, 1};
    Projection input_projection = knp::testing::DeltaProjection{
        knp::core::UID{false}, population.get_uid(), knp::testing::input_projection_gen, 1};
    knp::core::UID const input_uid = std::visit([](const auto &proj) { return proj.get_uid(); }, input_projection);

    backend.load_populations({population});
    backend.load_projections({input_projection, loop_projection});

    backend._init();
    auto endpoint = backend.get_message_bus().create_endpoint();

    const knp::core::UID in_channel_uid;
    const knp::core::UID out_channel_uid;

    // Create input and output
    backend.subscribe<knp::core::messaging::SpikeMessage>(input_uid, {in_channel_uid});
    endpoint.subscribe<knp::core::messaging::SpikeMessage>(out_channel_uid, {population.get_uid()});

    std::vector<knp::core::Step> results;

    for (knp::core::Step step = 0; step < 20; ++step)
    {
        // Send inputs on steps 0, 5, 10, 15
        if (step % 5 == 0)
        {
            knp::core::messaging::SpikeMessage message{{in_channel_uid, step}, {0}};
            endpoint.send_message(message);
        }
        backend._step();
        endpoint.receive_all_messages();
        // Write up the steps where the network sends a spike
        if (!endpoint.unload_messages<knp::core::messaging::SpikeMessage>(out_channel_uid).empty())
        {
            results.push_back(step);
        }
    }

    // Spikes on steps "5n + 1" (input) and on "previous_spike_n + 6" (positive feedback loop)
    const std::vector<knp::core::Step> expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};
    ASSERT_EQ(results, expected_results);
}


TEST(SingleThreadCpuSuite, AdditiveSTDPNetwork)
{
    namespace kt = knp::testing;

    using STDPDeltaProjection = knp::core::Projection<knp::synapse_traits::AdditiveSTDPDeltaSynapse>;

    // Create an STDP input projection
    auto stdp_input_projection_gen = [](size_t /*index*/) -> std::optional<STDPDeltaProjection::Synapse>
    {
        return STDPDeltaProjection::Synapse{
            {{1.0, 1, knp::synapse_traits::OutputType::EXCITATORY}, {.tau_plus_ = 2, .tau_minus_ = 2}}, 0, 0};
    };

    // Create an STDP loop projection
    auto stdp_synapse_generator = [](size_t /*index*/) -> std::optional<STDPDeltaProjection::Synapse>
    {
        return STDPDeltaProjection::Synapse{
            {{1.0, 6, knp::synapse_traits::OutputType::EXCITATORY}, {.tau_plus_ = 1, .tau_minus_ = 1}}, 0, 0};
    };

    auto stdp_neurons_generator = [](size_t /*index*/)  // NOLINT
        -> knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>
    { return knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>{}; };

    // Create a single neuron network: input -> input_projection -> population <=> loop_projection
    kt::STestingBack backend;

    knp::core::Population<knp::neuron_traits::BLIFATNeuron> population{knp::core::UID(), stdp_neurons_generator, 1};

    auto loop_projection = STDPDeltaProjection{population.get_uid(), population.get_uid(), stdp_synapse_generator, 1};
    Projection input_projection =
        STDPDeltaProjection{knp::core::UID{false}, population.get_uid(), stdp_input_projection_gen, 1};
    knp::core::UID input_uid = std::visit([](const auto &proj) { return proj.get_uid(); }, input_projection);

    loop_projection.get_shared_parameters().stdp_populations_[population.get_uid()] =
        STDPDeltaProjection::SharedSynapseParameters::ProcessingType::STDPAndSpike;

    backend.load_populations({population});
    backend.load_projections({input_projection, loop_projection});

    backend._init();
    auto endpoint = backend.get_message_bus().create_endpoint();

    knp::core::UID in_channel_uid;
    knp::core::UID out_channel_uid;

    // Create input and output
    backend.subscribe<knp::core::messaging::SpikeMessage>(input_uid, {in_channel_uid});
    endpoint.subscribe<knp::core::messaging::SpikeMessage>(out_channel_uid, {population.get_uid()});

    std::vector<knp::core::Step> results;

    for (knp::core::Step step = 0; step < 20; ++step)
    {
        // Send inputs on steps 0, 5, 10, 15
        if (step % 5 == 0)
        {
            knp::core::messaging::SpikeMessage message{{in_channel_uid, 0}, {0}};
            endpoint.send_message(message);
        }
        backend._step();
        endpoint.receive_all_messages();
        auto output = endpoint.unload_messages<knp::core::messaging::SpikeMessage>(out_channel_uid);
        // Write up the steps where the network sends a spike
        if (!output.empty()) results.push_back(step);
    }

    std::vector<float> old_synaptic_weights, new_synaptic_weights;
    old_synaptic_weights.reserve(loop_projection.size());
    new_synaptic_weights.reserve(loop_projection.size());

    std::transform(
        loop_projection.begin(), loop_projection.end(), std::back_inserter(old_synaptic_weights),
        // todo: replace 0 with "params".
        [](const auto &synapse) { return std::get<0>(synapse).weight_; });

    for (auto proj = backend.begin_projections(); proj != backend.end_projections(); ++proj)
    {
        const auto &prj = std::get<STDPDeltaProjection>(proj->arg_);
        if (prj.get_uid() != loop_projection.get_uid())
        {
            continue;
        }

        std::transform(
            prj.begin(), prj.end(), std::back_inserter(new_synaptic_weights),
            // todo: replace 0 with "params".
            [](const auto &synapse) { return std::get<0>(synapse).weight_; });
    }

    // Spikes on steps "5n + 1" (input) and on "previous_spike_n + 6" (positive feedback loop)
    const std::vector<knp::core::Step> expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};

    ASSERT_EQ(results, expected_results);
    ASSERT_NE(old_synaptic_weights, new_synaptic_weights);
}


TEST(SingleThreadCpuSuite, ResourceSTDPNetwork)
{
    // TODO: Remove code duplication.
    namespace kt = knp::testing;

    using STDPDeltaProjection = knp::core::Projection<knp::synapse_traits::SynapticResourceSTDPDeltaSynapse>;
    using BlifatStdpPopulation = knp::core::Population<knp::neuron_traits::SynapticResourceSTDPBLIFATNeuron>;

    // Create an STDP input projection
    auto stdp_input_projection_gen = [](size_t /*index*/) -> std::optional<STDPDeltaProjection::Synapse>
    {
        return STDPDeltaProjection::Synapse{
            {{1.0, 1, knp::synapse_traits::OutputType::EXCITATORY}, {.w_min_ = 1, .w_max_ = 2, .d_u_ = 0.1F}}, 0, 0};
    };

    // Create an STDP loop projection
    auto stdp_synapse_generator = [](size_t /*index*/) -> std::optional<STDPDeltaProjection::Synapse>
    {
        return STDPDeltaProjection::Synapse{
            {{1.0, 6, knp::synapse_traits::OutputType::EXCITATORY}, {.w_min_ = 1, .w_max_ = 2}}, 0, 0};
    };

    // Create a single neuron network: input -> input_projection -> population <=> loop_projection
    kt::STestingBack backend;

    BlifatStdpPopulation population{
        knp::core::UID(),
        [](uint64_t) -> std::optional<BlifatStdpPopulation::NeuronParameters>
        {
            BlifatStdpPopulation::NeuronParameters neuron{{}};
            neuron.synaptic_resource_threshold_ = 1;
            neuron.free_synaptic_resource_ = 2;
            neuron.isi_max_ = 0;
            return neuron;
        },
        1};
    auto loop_projection = STDPDeltaProjection{population.get_uid(), population.get_uid(), stdp_synapse_generator, 1};
    Projection input_projection =
        STDPDeltaProjection{knp::core::UID{false}, population.get_uid(), stdp_input_projection_gen, 1};
    const knp::core::UID input_uid = std::visit([](const auto &proj) { return proj.get_uid(); }, input_projection);

    backend.load_populations({population});
    backend.load_projections({input_projection, loop_projection});

    backend._init();
    backend.start_learning();
    auto endpoint = backend.get_message_bus().create_endpoint();

    const knp::core::UID in_channel_uid;
    const knp::core::UID out_channel_uid;

    // Create input and output
    backend.subscribe<knp::core::messaging::SpikeMessage>(input_uid, {in_channel_uid});
    endpoint.subscribe<knp::core::messaging::SpikeMessage>(out_channel_uid, {population.get_uid()});

    std::vector<knp::core::Step> results;

    for (knp::core::Step step = 0; step < 20; ++step)
    {
        // Send inputs on steps 0, 5, 10, 15
        if (step % 5 == 0)
        {
            knp::core::messaging::SpikeMessage message{{in_channel_uid, step}, {0}};
            endpoint.send_message(message);
        }
        backend._step();
        size_t msg_count = endpoint.receive_all_messages();
        SPDLOG_DEBUG("Received {} messages", msg_count);
        auto output = endpoint.unload_messages<knp::core::messaging::SpikeMessage>(out_channel_uid);
        SPDLOG_DEBUG("Unloaded {} messages", output.size());
        // Write up the steps where the network sends a spike
        if (!output.empty())
        {
            results.push_back(step);
        }
    }

    std::vector<float> old_synaptic_weights, new_synaptic_weights;
    old_synaptic_weights.reserve(loop_projection.size());
    new_synaptic_weights.reserve(loop_projection.size());

    std::transform(
        loop_projection.begin(), loop_projection.end(), std::back_inserter(old_synaptic_weights),
        // todo: replace 0 with "params".
        [](const auto &synapse) { return std::get<0>(synapse).weight_; });

    for (auto proj = backend.begin_projections(); proj != backend.end_projections(); ++proj)
    {
        const auto &prj = std::get<STDPDeltaProjection>(proj->arg_);
        if (prj.get_uid() != loop_projection.get_uid()) continue;

        std::transform(
            prj.begin(), prj.end(), std::back_inserter(new_synaptic_weights),
            // todo: replace 0 with "params".
            [](const auto &synapse) { return std::get<0>(synapse).weight_; });
    }

    // Spikes on steps "5n + 1" (input) and on "previous_spike_n + 6" (positive feedback loop)
    const std::vector<knp::core::Step> expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};

    ASSERT_EQ(results, expected_results);
    ASSERT_NE(old_synaptic_weights, new_synaptic_weights);
}


TEST(SingleThreadCpuSuite, NeuronsGettingTest)
{
    const knp::testing::STestingBack backend;

    auto s_neurons = backend.get_supported_neurons();

    ASSERT_LE(s_neurons.size(), boost::mp11::mp_size<knp::neuron_traits::AllNeurons>());
    ASSERT_EQ(s_neurons[0], "BLIFATNeuron");
}


TEST(SingleThreadCpuSuite, SynapsesGettingTest)
{
    const knp::testing::STestingBack backend;

    auto s_synapses = backend.get_supported_synapses();

    ASSERT_LE(s_synapses.size(), boost::mp11::mp_size<knp::synapse_traits::AllSynapses>());
    ASSERT_EQ(s_synapses[0], "DeltaSynapse");
}
