/**
 * @file backend.cpp
 * @brief Single-threaded CPU backend class implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */


#include <knp/backends/cpu-single-threaded/backend.h>

#include "blifat_population.h"
#include "delta_synapse_projection.h"


namespace knp::backends::single_threaded_cpu
{
void SingleThreadedCPUBackend::step() {}


void SingleThreadedCPUBackend::load_populations(const std::vector<PopulationVariants> &populations) {}


void SingleThreadedCPUBackend::load_populations(std::vector<PopulationVariants> &&populations) {}


void SingleThreadedCPUBackend::load_projections(const std::vector<ProjectionVariants> &projections) {}


void SingleThreadedCPUBackend::load_projections(const std::vector<ProjectionVariants> &&projections) {}


void SingleThreadedCPUBackend::calculate_population(knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population)
{
}


void SingleThreadedCPUBackend::calculate_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection)
{
}
}  // namespace knp::backends::single_threaded_cpu
