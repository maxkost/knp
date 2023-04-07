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

TEST(SingleThreadCpuSuite, SmallestNetwork)
{
    //    knp::backends::single_threaded_cpu::SingleThreadedCPUBackend backend;
    //
    //    auto neuron_generator = [](size_t)
    //    { return knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>{}; };
    //    BlifatPopulation population{neuron_generator, 1};
    //
    //    DeltaProjection::SynapseGenerator synapse_generator = [](size_t index) -> std::optional<DeltaProjection
    //    ::Synapse> {
    //        return DeltaProjection::Synapse{{1.0, 8}, 0, 0};
    //    };
    //    DeltaProjection projection{population.get_uid(), population.get_uid(), synapse_generator, 1};
    //
    //    backend.load_populations({population});
    //    backend.load_projections({projection});
    //    for (size_t step = 0; step < 1000; ++step)
    //    {
    //        backend.step();
    //    }
}
