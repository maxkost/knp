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
    /**
     * @brief Population variant that contains any population type specified in `SupportedPopulations`.
     * @details `PopulationVariants` takes the value of `std::variant<PopulationType_1,..., PopulationType_n>`, where `PopulationType_[1..n]` is the population type specified in `SupportedPopulations`. 
     * \n For example, if `SupportedPopulations` containes BLIFATNeuron and IzhikevichNeuron types, then `PopulationVariants = std::variant<BLIFATNeuron, IzhikevichNeuron>`. 
     * \n `PopulationVariants` retains the same order of message types as defined in `SupportedPopulations`.
     * @see ALL_NEURONS.
     */
    using PopulationVariants = boost::mp11::mp_rename<SupportedPopulations, std::variant>;
    /**
     * @brief Projection variant that contains any projection type specified in `SupportedProjections`.
     * @details `ProjectionVariants` takes the value of `std::variant<ProjectionType_1,..., ProjectionType_n>`, where `ProjectionType_[1..n]` is the projection type specified in `SupportedProjections`. 
     * \n For example, if `SupportedProjections` containes DeltaSynapse and AdditiveSTDPSynapse types, then `ProjectionVariants = std::variant<DeltaSynapse, AdditiveSTDPSynapse>`. 
     * \n `ProjectionVariants` retains the same order of message types as defined in `SupportedProjections`.
     * @see ALL_SYNAPSES.
     */
    using ProjectionVariants = boost::mp11::mp_rename<SupportedProjections, std::variant>;

private:
    struct ProjectionWrapper
    {
        ProjectionVariants arg;
        core::messaging::SynapticMessageQueue messages;
    };

    struct PopulationWrapper
    {
        PopulationVariants arg;
    };

public:
    // TODO: set protected (in testing purposes).
    SingleThreadedCPUBackend();
    ~SingleThreadedCPUBackend() = default;

public:
    static std::shared_ptr<SingleThreadedCPUBackend> create();

public:
    /**
     * @brief Load populations to the backend.
     * @param populations vector of populations to load.
     */
    void load_populations(const std::vector<PopulationVariants> &populations);

    /**
     * @brief Load projections to the backend.
     * @param projections vector of projections to load.
     */
    void load_projections(const std::vector<ProjectionVariants> &projections);

public:
    /**
     * @brief Remove projections with given UIDs from the backend.
     * @param uids UIDs of projections to remove.
     */
    void remove_projections(const std::vector<knp::core::UID> &uids) override {}

    /**
     * @brief Remove synapses of the projection with the given UID from the backend.
     * @param projection_uid projection UID.
     * @param indexes indexes of synapses to remove.
     */
    void remove_synapses(const knp::core::UID &projection_uid, const std::vector<size_t> &indexes) override {}

    /**
     * @brief Remove populations with given UIDs from the backend.
     * @param uids UIDs of populations to remove.
     */
    void remove_populations(const std::vector<knp::core::UID> &uids) override {}

public:
    /**
     * @brief Get a list of devices supported by the backend.
     * @return list of devices.
     * @see Device.
     */
    [[nodiscard]] std::vector<std::unique_ptr<knp::core::Device>> get_devices() const override;

public:
    void step() override;

    /**
     * @brief Subscribe internal endpoint to messages. Needed to send messages into the network
     * @tparam MessageType Message type
     * @param receiver Receiving object UID
     * @param senders a list of possible senders
     * @return subscription
     */
    template <typename MessageType>
    knp::core::Subscription<MessageType> &subscribe(
        const knp::core::UID &receiver, const std::vector<knp::core::UID> &senders)
    {
        return message_endpoint_.subscribe<MessageType>(receiver, senders);
    }

protected:
    void init();

    /**
     * @brief Calculate the population of BLIFAT neurons.
     * @note Population will be changed during calculation.
     * @param population population of BLIFAT neurons to calculate.
     * @param wrapper population wrapper
     */
    void calculate_population(
        knp::core::Population<knp::neuron_traits::BLIFATNeuron> &population, PopulationWrapper &wrapper);
    /**
     * @brief Calculate the projection of Delta synapses.
     * @note Projection will be changed during calculation.
     * @param projection projection of Delta synapses to calculate.
     * @param wrapper projection wrapper
     */
    void calculate_projection(
        knp::core::Projection<knp::synapse_traits::DeltaSynapse> &projection, ProjectionWrapper &wrapper);

private:
    template <typename TypeList, auto CalculateMethod, typename Container>
    inline void calculator(Container &container);

private:
    std::vector<PopulationWrapper> populations_;
    std::vector<ProjectionWrapper> projections_;
    core::MessageEndpoint message_endpoint_;
    size_t step_ = 0;
};


BOOST_DLL_ALIAS(knp::backends::single_threaded_cpu::SingleThreadedCPUBackend::create, create_knp_backend)

}  // namespace knp::backends::single_threaded_cpu
