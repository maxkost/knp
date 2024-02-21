/**
 * @file stdp_synaptic_resource_rule.h
 * @brief Synaptic resource-based STDP neuron parameters.
 * @author Artiom N.
 * @date 06.10.2023
 */

#pragma once

#include <cinttypes>
#include <limits>


/**
 * @brief Namespace for neuron traits.
 */
namespace knp::neuron_traits
{
/**
 * @brief Neuron type with resource-based STDP learning rule.
 * @details You can use this structure as a wrapper over base neuron type.
 * @tparam NeuronType base neuron type.
 */
template <typename NeuronType>
struct SynapticResourceSTDPNeuron;

/**
 * @brief Neuron state in the continuous spike generation period.
 */
enum class ISIPeriodType
{
    /**
     * @brief Neuron is forced, i.e. spiked while receiving an excitatory signal from a non-plastic neuron.
     */
    is_forced,
    /**
     * @brief Neuron receives the first non-forced spike.
     */
    period_started,
    /**
     * @brief Neuron continues to receive non-forced spikes.
     */
    period_continued,
    /**
     * @brief Sequence of spikes has ended. A new sequence has not started yet.
     * @note The state is not commonly used as neuron state changes after a spike.
     */
    not_in_period
};


/**
 * @brief Template for neuron parameters supported by synaptic resource-based STDP.
 * @tparam NeuronType base neuron type without plasticity.
 */
template <typename NeuronType>
struct default_values<SynapticResourceSTDPNeuron<NeuronType>>
{
    /**
     * @brief Base neuron default parameters.
     */
    default_values<NeuronType> neuron_;
};


/**
 * @brief Template for neuron parameters supported by synaptic resource-based STDP.
 * @tparam NeuronType base neuron type without plasticity.
 * @note The class contains all neuron parameters of the base neuron.
 */
template <typename NeuronType>
struct neuron_parameters<SynapticResourceSTDPNeuron<NeuronType>> : public neuron_parameters<NeuronType>
{
    /**
     * @brief Construct parameters for synaptic resource-based STDP from a base neuron.
     * @param base_neuron starting parameters of the base neuron type.
     */
    explicit neuron_parameters(const neuron_parameters<NeuronType> &base_neuron)
        : neuron_parameters<NeuronType>(base_neuron)
    {
    }
    /**
     * @brief Free synaptic resource.
     */
    float free_synaptic_resource_ = 1;
    /**
     * @brief Synaptic resource threshold value.
     */
    float synaptic_resource_threshold_ = std::numeric_limits<float>::infinity();
    /**
     * @brief Synaptic resource divided by `number of synapses + resource drain coefficient`.
     */
    uint32_t resource_drain_coefficient_ = 0;
    /**
     * @brief Stability.
     */
    float stability_ = 0;

    /**
     * @brief The parameter defines the stability fluctuation value.
     */
    float stability_change_parameter_ = 0;

    /**
     * @brief This value is added to stability at the beginning of each ISI period.
     */
    float stability_change_at_isi_ = 0;
    /**
     * @brief Time between spikes in the ISI period.
     */
    uint32_t isi_max_ = 1;
    /**
     * @brief Hebbian plasticity value.
     */
    float d_h_ = 1.F;
    /**
     * @brief ISI period status.
     */
    ISIPeriodType isi_status_ = ISIPeriodType::period_continued;
    /**
     * @brief Last non-forced spike step.
     */
    uint64_t last_step_ = 0;

    /**
     * @brief Step of the last unforced spike. Used to update the dopamine value.
     */
    uint64_t first_isi_spike_ = 0;

    /**
     * @brief `true` if a neuron receives a forced synaptic signal.
     */
    bool is_being_forced_ = false;
};

}  // namespace knp::neuron_traits
