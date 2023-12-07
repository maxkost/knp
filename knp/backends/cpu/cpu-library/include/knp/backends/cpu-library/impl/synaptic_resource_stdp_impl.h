/**
 * @file synaptic_resource_stdp.h
 * @brief Synaptci resource based STDP neurons adapter.
 * @author Artiom N.
 * @date 06.10.2023
 */

#pragma once

#include <limits>
#include <numeric>
#include <utility>

#include <boost/mp11.hpp>

#include "knp/neuron-traits/type_traits.h"

/**
 * @brief Namespace for neuron traits.
 */
namespace knp::neuron_traits
{


/**
 * @brief Update spike sequence state for the neuron. It's called after a neuron sends a spike.
 * @tparam NeuronType base neuron type.
 * @param neuron neuron parameters.
 * @param step current step.
 * @return new state.
 */
template <class NeuronType>
ISIPeriodType update_isi(neuron_parameters<SynapticResourceSTDPNeuron<NeuronType>> &neuron, uint64_t step)
{
    if (neuron.is_being_forced_)  // This neuron got a forcing spike this turn and doesn't continue its spiking sequence
    {
        neuron.isi_status_ = neuron_traits::ISIPeriodType::is_forced;
        // Do not update last_step_
        return neuron.isi_status_;
    }
    else
    {
        switch (neuron.isi_status_)
        {
            case neuron_traits::ISIPeriodType::is_forced:
                neuron.isi_status_ = neuron_traits::ISIPeriodType::period_started;
                neuron.first_isi_spike_ = step;
                break;
            case neuron_traits::ISIPeriodType::period_started:
                if (neuron.last_step_ - step < neuron.isi_max_)
                    neuron.isi_status_ = neuron_traits::ISIPeriodType::period_continued;
                break;
            case neuron_traits::ISIPeriodType::period_continued:
                if (neuron.last_step_ - step >= neuron.isi_max_ || neuron.dopamine_value_ != 0)
                {
                    neuron.isi_status_ = neuron_traits::ISIPeriodType::period_started;
                    neuron.first_isi_spike_ = step;
                }
                break;
            case neuron_traits::ISIPeriodType::not_in_period:
                neuron.isi_status_ = neuron_traits::ISIPeriodType::period_started;
                neuron.first_isi_spike_ = step;
                break;
            default:
                throw std::runtime_error("Not supported ISI status.");
        }
    }
    neuron.last_step_ = step;
    return neuron.isi_status_;
}

}  // namespace knp::neuron_traits
