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

    using PopulationVariants = boost::mp11::mp_rename<SupportedPopulations, std::variant>;
    using ProjectionVariants = boost::mp11::mp_rename<SupportedProjections, std::variant>;

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

public:
    void step() override;

protected:
    /**
     * @brief calculate BLIFAT neurons population.
     * Projection will be changed during calculation.
     * @param population.
     */
    void calculate_population(knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population);
    /**
     * @brief calculate DeltaSynapse projection.
     * Projection will be changed during calculation.
     * @param projection.
     */
    void calculate_projection(knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection);

private:
    template <typename TypeList, auto CalculateMethod, typename Container>
    inline void calculator(Container &container);

private:
    knp::devices::CPU device_;
    std::vector<PopulationVariants> populations_;
    std::vector<ProjectionVariants> projections_;
};

}  // namespace knp::backends::single_threaded_cpu
