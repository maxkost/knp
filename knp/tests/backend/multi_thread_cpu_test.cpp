/**
 * @brief Multithreading backend test.
 * @author Vartenkov An.
 * @date 07.04.23.
 */

#include <knp/backends/cpu-multi-threaded/backend.h>
#include <knp/backends/thread_pool/thread_pool_context.h>
#include <knp/backends/thread_pool/thread_pool_executor.h>
#include <knp/core/population.h>
#include <knp/core/projection.h>

#include <generators.h>
#include <tests_common.h>

#include <functional>
#include <vector>


using Population = knp::backends::multi_threaded_cpu::MultiThreadedCPUBackend::PopulationVariants;
using Projection = knp::backends::multi_threaded_cpu::MultiThreadedCPUBackend::ProjectionVariants;


namespace knp::testing
{

class MTestingBack : public knp::backends::multi_threaded_cpu::MultiThreadedCPUBackend
{
public:
    MTestingBack() = default;
    void init() override { knp::backends::multi_threaded_cpu::MultiThreadedCPUBackend::init(); }
};

}  // namespace knp::testing


TEST(MultiThreadCpuSuite, SmallestNetwork)
{
    // Create a single neuron network: input -> input_projection -> population <=> loop_projection.

    namespace kt = knp::testing;
    kt::MTestingBack backend;

    kt::BLIFATPopulation population{kt::neuron_generator, 1};
    Projection loop_projection =
        kt::DeltaProjection{population.get_uid(), population.get_uid(), kt::synapse_generator, 1};
    Projection input_projection =
        kt::DeltaProjection{knp::core::UID{false}, population.get_uid(), kt::input_projection_gen, 1};
    knp::core::UID input_uid = std::visit([](const auto &proj) { return proj.get_uid(); }, input_projection);

    backend.load_populations({population});
    backend.load_projections({input_projection, loop_projection});

    auto endpoint = backend.message_bus_.create_endpoint();

    knp::core::UID in_channel_uid;
    knp::core::UID out_channel_uid;

    // Create input and output.
    backend.subscribe<knp::core::messaging::SpikeMessage>(input_uid, {in_channel_uid});
    endpoint.subscribe<knp::core::messaging::SpikeMessage>(out_channel_uid, {population.get_uid()});

    std::vector<knp::core::Step> results;

    backend.init();

    for (knp::core::Step step = 0; step < 20; ++step)
    {
        // Send inputs on steps 0, 5, 10, 15.
        if (step % 5 == 0)
        {
            knp::core::messaging::SpikeMessage message{{in_channel_uid, 0}, {0}};
            endpoint.send_message(message);
        }
        backend.step();
        endpoint.receive_all_messages();
        auto output = endpoint.unload_messages<knp::core::messaging::SpikeMessage>(out_channel_uid);
        // Write up the steps where the network sends a spike.
        if (!output.empty()) results.push_back(step);
    }

    // Spikes on steps "5n + 1" (input) and on "previous_spike_n + 6" (positive feedback loop).
    const std::vector<knp::core::Step> expected_results = {1, 6, 7, 11, 12, 13, 16, 17, 18, 19};
    ASSERT_EQ(results, expected_results);
}


TEST(MultiThreadCpuSuite, NeuronsGettingTest)
{
    knp::testing::MTestingBack backend;

    auto s_neurons = backend.get_supported_neurons();

    ASSERT_GE(s_neurons.size(), 1);
    ASSERT_EQ(s_neurons[0], "knp::neuron_traits::BLIFATNeuron");
}


TEST(MultiThreadCpuSuite, SynapsesGettingTest)
{
    knp::testing::MTestingBack backend;

    auto s_synapses = backend.get_supported_synapses();

    ASSERT_GE(s_synapses.size(), 1);
    ASSERT_EQ(s_synapses[0], "knp::synapse_traits::DeltaSynapse");
}


void fibonacci(const uint64_t begin, uint64_t iterations, uint64_t *result)
{
    // This function calculates last 3 digits of "begin * Fibonacci(iterations)".
    uint64_t prev[] = {begin, 0};
    const uint64_t divider = 1000;  // 10^N, used so that only N last digits remain.
    for (uint64_t i = 0; i < iterations; ++i)
    {
        const auto tmp = (prev[0] + prev[1]) % divider;
        prev[1] = prev[0];
        prev[0] = tmp;
    }
    *result = prev[0];
}


void batch(
    knp::backends::cpu_executors::ThreadPoolContext &pool, uint64_t iterations,
    const std::vector<uint64_t> &start_values, std::vector<uint64_t> &result)
{
    knp::backends::cpu_executors::ThreadPoolExecutor executor(pool);
    result.resize(start_values.size(), 0);
    for (size_t i = 0; i < start_values.size(); ++i)
        boost::asio::post(executor, std::bind(fibonacci, start_values[i], iterations, &result[i]));
}


TEST(MultiThreadCpuSuite, ThreadPoolTest)
{
    knp::backends::cpu_executors::ThreadPoolContext pool;
    std::vector<uint64_t> result;
    const int num_iterations = 10;  // Corresponding fibonacci number is 89.
    batch(pool, num_iterations, {2, 4, 5, 7, 9}, result);
    ASSERT_EQ(result.size(), 5);  // All threads are finished.
    ASSERT_EQ(result[0], 178);    // The results are correct (89 * 2).
    ASSERT_EQ(result[1], 356);

    // Check that pool is reusable.
    batch(pool, num_iterations, {7, 5, 5, 7, 9, 11, 8, 7}, result);
    ASSERT_EQ(result.size(), 8);
    ASSERT_EQ(result[0], 623);
    ASSERT_EQ(result[1], 445);
    ASSERT_EQ(result[0], result[7]);  // Delayed tasks should give the same results as the first ones.
}
