/**
 * @file stdp_synaptic_resource_rule.h
 * @brief Synaptcic resource based STDP neurons parameters.
 * @author Artiom N.
 * @date 06.10.2023
 */

#pragma once
#include <limits>
/**
 * @brief Namespace for neuron traits.
 */
namespace knp::neuron_traits
{
/**
 * @brief A type of neuron with SynapticResourceSTDP learning rule. It is a wrapper over base neuron type.
 * @tparam NeuronType base neuron type.
 */
template <typename NeuronType>
struct SynapticResourceSTDPNeuron;

/**
 * @brief A state of neuron in continuous spike generation period.
 */
enum class ISIPeriodType
{
    /**
     * @brief Neuron is forced (spiked while receiving an excitatory signal from a non-plastic neuron).
     */
    is_forced,
    /**
     * @brief A first non-forced spike in a period.
     */
    period_started,
    /**
     * @brief Non-forced spikes following the first one.
     */
    period_continued,
    /**
     * @brief Previous sequence has ended, a new one hasn't started yet. Not really used as state changes on a spike.
     */
    not_in_period
};


/**
 * @brief Template for the neuron parameters, which are supported synaptic resource based STDP.
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
 * @brief Template for the neurons, which are supported synaptic resource based STDP.
 * @tparam NeuronType a base neuron type with no plasticity.
 * @note This class contains all neuron parameters of the base neuron.
 */
template <typename NeuronType>
struct neuron_parameters<SynapticResourceSTDPNeuron<NeuronType>> : public neuron_parameters<NeuronType>
{
    /**
     * @brief Construct a SynapticResourceSTDP parameters from a base neuron.
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
     * @brief Synaptic resource is divided by (number of synapses + resource drain coefficient).
     */
    uint32_t resource_drain_coefficient_ = 0;
    /**
     * @brief Stability.
     */
    float stability_ = 0;

    /**
     * @brief A parameter that defines value of stability changes.
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
     * @brief Step of last unforced spike. Used for dopamine update.
     */
    uint64_t first_isi_spike_ = 0;

    /**
     * @brief internal parameter. True if a neuron received a forcing synaptic signal on this turn.
     */
    bool is_being_forced_ = false;
};

}  // namespace knp::neuron_traits
