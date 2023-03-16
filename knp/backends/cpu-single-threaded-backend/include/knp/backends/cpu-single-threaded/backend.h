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

#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include <boost/config.hpp>
#include <boost/dll/alias.hpp>
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
    // TODO: set protected (in testing purposes).
    SingleThreadedCPUBackend() : message_endpoint_{message_bus_.get_endpoint()} {}

public:
    static std::shared_ptr<SingleThreadedCPUBackend> create() { return std::make_shared<SingleThreadedCPUBackend>(); }

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
    /**
     * @brief Remove projections with given UIDs from the backend.
     * @param uids identifiers of the projections, which will be removed.
     */
    void remove_projections(const std::vector<knp::core::UID> &uids) override {}

    /**
     * @brief Remove synapses from the projection with given UID from the backend.
     * @param projection_uid identifiers of the projection.
     * @param indexes synapses indexes to remove.
     */
    void remove_synapses(const knp::core::UID &projection_uid, const std::vector<size_t> &indexes) override {}

    /**
     * @brief Remove populations with given UIDs from the backend.
     * @param uids identifiers of the populations, which will be removed.
     */
    void remove_populations(const std::vector<knp::core::UID> &uids) override {}

public:
    /**
     * @brief Get devices list, supported by the backend.
     * @return list of the devices.
     * @see Device.
     */
    [[nodiscard]] std::vector<std::unique_ptr<knp::core::Device>> get_devices() const override;

public:
    void step() override;

protected:
    void init();

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
    std::vector<PopulationVariants> populations_;
    std::vector<ProjectionVariants> projections_;
    core::MessageEndpoint message_endpoint_;
};


BOOST_DLL_ALIAS(knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::create, create_backend)

}  // namespace knp::backends::single_threaded_cpu
