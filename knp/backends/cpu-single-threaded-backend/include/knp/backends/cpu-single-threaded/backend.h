/**
 * @file backend.h
 * @brief Single threaded CPU backend class definition.
 * @author Artiom N.
 * @date 30.01.2023
 */

#pragma once

#include <knp/core/backend.h>
#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/core/type_list.h>
#include <knp/devices/cpu.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <variant>
#include <vector>

#include <boost/mp11.hpp>


namespace knp::backends::single_threaded_cpu
{

class SingleThreadedCPUBackend : public knp::core::Backend
{
public:
    using SupportedNeurons = boost::mp11::mp_list<knp::neuron_traits::BLIFATNeuron>;
    using SupportedSynapses = boost::mp11::mp_list<knp::synapse_traits::DeltaSynapse>;
    using SupportedPopulations = boost::mp11::mp_transform<knp::core::Population, SupportedNeurons>;
    using SupportedProjections = boost::mp11::mp_transform<knp::core::Projection, SupportedSynapses>;

    using PopulationVariants = boost::mp11::mp_fill<std::variant, SupportedPopulations>;
    using ProjectionVariants = boost::mp11::mp_fill<std::variant, SupportedProjections>;

public:
    // SingleThreadedCPUBackend(knp::devices::CPU &&cpu);

public:
    /**
     * @brief load populations to the backend.
     * @param populations vector to load.
     */
    void load_populations(const std::vector<PopulationVariants> &populations);

    /**
     * @brief load populations to the backend.
     * @param populations vector to load.
     */
    void load_populations(std::vector<PopulationVariants> &&populations);

    /**
     * @brief load projections to the backend.
     * @param projections vector to load.
     */
    void load_projections(const std::vector<ProjectionVariants> &projections);

    /**
     * @brief load projections to the backend.
     * @param projections vector to load.
     */
    void load_projections(const std::vector<ProjectionVariants> &&projections);

private:
    knp::devices::CPU device_;
};

}  // namespace knp::backends::single_threaded_cpu
