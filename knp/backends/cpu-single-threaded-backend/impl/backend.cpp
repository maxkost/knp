/**
 * @file backend.cpp
 * @brief Single-threaded CPU backend class implementation.
 * @author Artiom N.
 * @date 21.02.2023
 */


#include <knp/backends/cpu-single-threaded/backend.h>
#include <knp/core/core.h>

#include <boost/mp11.hpp>

#include "blifat_population.h"
#include "delta_synapse_projection.h"


namespace knp::backends::single_threaded_cpu
{

void SingleThreadedCPUBackend::step()
{
    // Calculate projections.
    // TODO: Make template.
    for (auto &p : projections_)
    {
        std::visit(
            [this](auto &projection)
            {
                using T = std::decay_t<decltype(projection)>;
                if constexpr (
                    boost::mp11::mp_find<SupportedProjections, T>{} == boost::mp11::mp_size<SupportedProjections>{})
                    static_assert(knp::core::always_false_v<T>, "Backend doesn't support this projection type!");
                calculate_projection(projection);
            },
            p);
    }

    // Calculate populations.
    for (auto &p : populations_)
    {
        std::visit(
            [this](auto &population)
            {
                using T = std::decay_t<decltype(population)>;
                if constexpr (
                    boost::mp11::mp_find<SupportedPopulations, T>{} == boost::mp11::mp_size<SupportedPopulations>{})
                    static_assert(knp::core::always_false_v<T>, "Backend doesn't support this population type!");
                calculate_population(population);
            },
            p);
    }
}


void SingleThreadedCPUBackend::load_populations(const std::vector<PopulationVariants> &populations) {}


void SingleThreadedCPUBackend::load_populations(std::vector<PopulationVariants> &&populations) {}


void SingleThreadedCPUBackend::load_projections(const std::vector<ProjectionVariants> &projections) {}


void SingleThreadedCPUBackend::load_projections(const std::vector<ProjectionVariants> &&projections) {}


void SingleThreadedCPUBackend::calculate_population(knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population)
{
    calculate_blifat_population(population, message_bus_);
}


void SingleThreadedCPUBackend::calculate_projection(
    knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection)
{
    calculate_delta_synapse_projection(projection, message_bus_);
}
}  // namespace knp::backends::single_threaded_cpu
