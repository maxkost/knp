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
    void init() override { knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::init(); }
};

}  // namespace knp::testing


TEST(SingleThreadCpuSuite, SmallestNetwork)
{
    namespace kt = knp::testing;

    // Create a single neuron network: input -> input_projection -> population <=> loop_projection
    kt::STestingBack backend;

    kt::BLIFATPopulation population{kt::neuron_generator, 1};
    Projection loop_projection =
        kt::DeltaProjection{population.get_uid(), population.get_uid(), kt::synapse_generator, 1};
    Projection input_projection =
        kt::DeltaProjection{knp::core::UID{false}, population.get_uid(), kt::input_projection_gen, 1};
    knp::core::UID input_uid = std::visit([](const auto &proj) { return proj.get_uid(); }, input_projection);

    backend.load_populations({population});
    backend.load_projections({input_projection, loop_projection});

    backend.init();
    auto endpoint = backend.message_bus_.create_endpoint();

    knp::core::UID in_channel_uid;
    knp::core::UID out_channel_uid;

    // Create input and output
    backend.subscribe<knp::core::messaging::SpikeMessage>(input_uid, {in_channel_uid});
    endpoint.subscribe<knp::core::messaging::SpikeMessage>(out_channel_uid, {population.get_uid()});

    std::vector<knp::core::messaging::Step> results;

    for (knp::core::messaging::Step step = 0; step < 20; ++step)
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
    const std::vector<knp::core::messaging::Step> expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};
    ASSERT_EQ(results, expected_results);
}
//
//
// TEST(SingleThreadCpuSuite, AdditiveSTDPNetwork)
//{
//    namespace kt = knp::testing;
//
//    using STDPDeltaProjection = knp::core::Projection<knp::synapse_traits::AdditiveSTDPDeltaSynapse>;
//
//    // Create an STDP input projection
//    auto stdp_input_projection_gen = [](size_t /*index*/) -> std::optional<STDPDeltaProjection::Synapse>
//    {
//        return STDPDeltaProjection::Synapse{
//            {{.tau_plus_ = 2, .tau_minus_ = 2}, {1.0, 1, knp::synapse_traits::OutputType::EXCITATORY}}, 0, 0};
//    };
//
//    // Create an STDP loop projection
//    auto stdp_synapse_generator = [](size_t /*index*/) -> std::optional<STDPDeltaProjection::Synapse>
//    {
//        return STDPDeltaProjection::Synapse{
//            {{.tau_plus_ = 1, .tau_minus_ = 1}, {1.0, 6, knp::synapse_traits::OutputType::EXCITATORY}}, 0, 0};
//    };
//
//    auto stdp_neurons_generator = [](size_t /*index*/)  // NOLINT
//        -> knp::neuron_traits::neuron_parameters<knp::neuron_traits::SynapticResourceSTDPBLIFATNeuron>
//    { return knp::neuron_traits::neuron_parameters<knp::neuron_traits::SynapticResourceSTDPBLIFATNeuron>{}; };
//
//    // Create a single neuron network: input -> input_projection -> population <=> loop_projection
//    kt::STestingBack backend;
//
//    knp::core::Population<knp::neuron_traits::SynapticResourceSTDPBLIFATNeuron> population{
//        knp::core::UID(), stdp_neurons_generator, 1};
//
//    auto loop_projection = STDPDeltaProjection{population.get_uid(), population.get_uid(), stdp_synapse_generator, 1};
//    Projection input_projection =
//        STDPDeltaProjection{knp::core::UID{false}, population.get_uid(), stdp_input_projection_gen, 1};
//    knp::core::UID input_uid = std::visit([](const auto &proj) { return proj.get_uid(); }, input_projection);
//
//    loop_projection.get_shared_parameters().stdp_populations_[population.get_uid()] =
//        STDPDeltaProjection::SharedSynapseParameters::ProcessingType::STDPAndSpike;
//
//    backend.load_populations({population});
//    backend.load_projections({input_projection, loop_projection});
//
//    backend.init();
//    auto endpoint = backend.message_bus_.create_endpoint();
//
//    knp::core::UID in_channel_uid;
//    knp::core::UID out_channel_uid;
//
//    // Create input and output
//    backend.subscribe<knp::core::messaging::SpikeMessage>(input_uid, {in_channel_uid});
//    endpoint.subscribe<knp::core::messaging::SpikeMessage>(out_channel_uid, {population.get_uid()});
//
//    std::vector<knp::core::messaging::Step> results;
//
//    for (knp::core::messaging::Step step = 0; step < 20; ++step)
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
//
//    std::vector<float> old_synaptic_weights, new_synaptic_weights;
//    old_synaptic_weights.reserve(loop_projection.size());
//    new_synaptic_weights.reserve(loop_projection.size());
//
//    std::transform(
//        loop_projection.begin(), loop_projection.end(), std::back_inserter(old_synaptic_weights),
//        [](const auto &s) { return s.params_.synapse_.weight_; });
//
//    for (auto p = backend.begin_projections(); p != backend.end_projections(); ++p)
//    {
//        const auto &prj = std::get<STDPDeltaProjection>(p->arg_);
//        if (prj.get_uid() != loop_projection.get_uid()) continue;
//
//        std::transform(
//            prj.begin(), prj.end(), std::back_inserter(new_synaptic_weights),
//            [](const auto &s) { return s.params_.synapse_.weight_; });
//    }
//
//    // Spikes on steps "5n + 1" (input) and on "previous_spike_n + 6" (positive feedback loop)
//    const std::vector<knp::core::messaging::Step> expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};
//
//    ASSERT_EQ(results, expected_results);
//    ASSERT_NE(old_synaptic_weights, new_synaptic_weights);
//}

//
// TEST(SingleThreadCpuSuite, ResourceSTDPNetwork)
//{
//    // TODO: Remove code duplication.
//    namespace kt = knp::testing;
//
//    using STDPDeltaProjection = knp::core::Projection<knp::synapse_traits::SynapticResourceSTDPDeltaSynapse>;
//
//    // Create an STDP input projection
//    auto stdp_input_projection_gen = [](size_t /*index*/) -> std::optional<STDPDeltaProjection::Synapse>
//    {
//        return STDPDeltaProjection::Synapse{
//            {{.w_min_ = 1, .w_max_ = 2}, {1.0, 1, knp::synapse_traits::OutputType::EXCITATORY}}, 0, 0};
//    };
//
//    // Create an STDP loop projection
//    auto stdp_synapse_generator = [](size_t /*index*/) -> std::optional<STDPDeltaProjection::Synapse>
//    {
//        return STDPDeltaProjection::Synapse{
//            {{.w_min_ = 1, .w_max_ = 2}, {1.0, 6, knp::synapse_traits::OutputType::EXCITATORY}}, 0, 0};
//    };
//
//    // Create a single neuron network: input -> input_projection -> population <=> loop_projection
//    kt::STestingBack backend;
//
//    kt::BLIFATPopulation population{knp::core::UID(), kt::neuron_generator, 1};
//    auto loop_projection = STDPDeltaProjection{population.get_uid(), population.get_uid(), stdp_synapse_generator, 1};
//    Projection input_projection =
//        STDPDeltaProjection{knp::core::UID{false}, population.get_uid(), stdp_input_projection_gen, 1};
//    knp::core::UID input_uid = std::visit([](const auto &proj) { return proj.get_uid(); }, input_projection);
//
//    loop_projection.get_shared_parameters().stdp_populations_[population.get_uid()] =
//        STDPDeltaProjection::SharedSynapseParameters::ProcessingType::STDPAndSpike;
//
//    backend.load_populations({population});
//    backend.load_projections({input_projection, loop_projection});
//
//    backend.init();
//    auto endpoint = backend.message_bus_.create_endpoint();
//
//    knp::core::UID in_channel_uid;
//    knp::core::UID out_channel_uid;
//
//    // Create input and output
//    backend.subscribe<knp::core::messaging::SpikeMessage>(input_uid, {in_channel_uid});
//    endpoint.subscribe<knp::core::messaging::SpikeMessage>(out_channel_uid, {population.get_uid()});
//
//    std::vector<knp::core::messaging::Step> results;
//
//    for (knp::core::messaging::Step step = 0; step < 20; ++step)
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
//
//    std::vector<float> old_synaptic_weights, new_synaptic_weights;
//    old_synaptic_weights.reserve(loop_projection.size());
//    new_synaptic_weights.reserve(loop_projection.size());
//
//    std::transform(
//        loop_projection.begin(), loop_projection.end(), std::back_inserter(old_synaptic_weights),
//        [](const auto &s) { return s.params_.synapse_.weight_; });
//
//    for (auto p = backend.begin_projections(); p != backend.end_projections(); ++p)
//    {
//        const auto &prj = std::get<STDPDeltaProjection>(p->arg_);
//        if (prj.get_uid() != loop_projection.get_uid()) continue;
//
//        std::transform(
//            prj.begin(), prj.end(), std::back_inserter(new_synaptic_weights),
//            [](const auto &s) { return s.params_.synapse_.weight_; });
//    }
//
//    // Spikes on steps "5n + 1" (input) and on "previous_spike_n + 6" (positive feedback loop)
//    const std::vector<knp::core::messaging::Step> expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};
//
//    ASSERT_EQ(results, expected_results);
//    // ASSERT_NE(old_synaptic_weights, new_synaptic_weights);
//}


TEST(SingleThreadCpuSuite, NeuronsGettingTest)
{
    knp::testing::STestingBack backend;

    auto s_neurons = backend.get_supported_neurons();

    ASSERT_GE(s_neurons.size(), 1);
    ASSERT_EQ(s_neurons[0], "knp::neuron_traits::BLIFATNeuron");
}


TEST(SingleThreadCpuSuite, SynapsesGettingTest)
{
    knp::testing::STestingBack backend;

    auto s_synapses = backend.get_supported_synapses();

    ASSERT_GE(s_synapses.size(), 1);
    ASSERT_EQ(s_synapses[0], "knp::synapse_traits::DeltaSynapse");
}
