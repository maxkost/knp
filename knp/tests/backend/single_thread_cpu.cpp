//
// Created by Vartenkov An. on 07.04.23.
//
#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <tests_common.h>

using DeltaProjection = knp::core::Projection<knp::synapse_traits::DeltaSynapse>;
using BlifatPopulation = knp::core::Population<knp::neuron_traits::BLIFATNeuron>;
using Population = knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::PopulationVariants;
using Projection = knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::ProjectionVariants;

TEST(SingleThreadCpuSuite, SmallestNetwork)
{
    knp::backends::single_threaded_cpu::SingleThreadedCPUBackend backend;
    knp::core::UID input_uid{true};

    // Create a single-neuron population
    auto neuron_generator = [](size_t i)
    { return knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>{}; };
    BlifatPopulation population{neuron_generator, 1};

    // Create a loop projection
    DeltaProjection::SynapseGenerator synapse_generator = [](size_t index) -> std::optional<DeltaProjection ::Synapse> {
        return DeltaProjection::Synapse{{1.0, 8}, 0, 0};
    };
    Projection loop_projection = DeltaProjection{population.get_uid(), population.get_uid(), synapse_generator, 1};

    // Create an input projection
    DeltaProjection::SynapseGenerator input_projection_gen = [](size_t index) -> std::optional<DeltaProjection::Synapse>
    {
        return DeltaProjection::Synapse{{1.0, 1}, 0, 0};
    };
    Projection input_projection = DeltaProjection{input_uid, population.get_uid(), input_projection_gen, 1};

    backend.load_populations({population});

    backend.load_projections({input_projection, loop_projection});

    auto endpoint = backend.message_bus_.create_endpoint();
    knp::core::UID in_channel_uid{true};
    endpoint.subscribe<knp::core::messaging::SpikeMessage>(input_uid, {in_channel_uid});
    for (size_t step = 0; step < 1000; ++step)
    {
        if (step % 5 == 0)
        {
            knp::core::messaging::SpikeMessage message{{in_channel_uid, 0}, {0}};
            endpoint.send_message(message);
        }
        backend.step();
    }
}
