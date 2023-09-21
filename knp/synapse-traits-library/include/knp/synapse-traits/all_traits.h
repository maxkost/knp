/**
 * @file delta.h
 * @brief Delta synapse types traits.
 * @author Artiom N.
 * @date 22.03.2023
 */

#pragma once

#include <knp/meta/stringify.h>

#include <unordered_map>

#include <boost/mp11.hpp>

#include "delta.h"
#include "stdp.h"

/**
 * @brief Namespace for synapse traits.
 */
namespace knp::synapse_traits
{

// Comma-separated list of synapses.
#define ALL_SYNAPSES knp::synapse_traits::DeltaSynapse, knp::synapse_traits::AdditiveSTDPDeltaSynapse
/**
 * @brief List of synapse types.
 * @details To add a new synapse type to the list, define it in the ALL_SYNAPSES macro. For example,
 * @code{.cpp}
 * #define ALL_SYNAPSES knp::synapse_traits::DeltaSynapse, knp::synapse_traits::AdditiveSTDPDeltaSynapse
 * @endcode
 */
using AllSynapses = boost::mp11::mp_list<ALL_SYNAPSES>;

/**
 * @brief A tuple that contains string names of synapse types.
 */
constexpr auto synapses_names = KNP_MAKE_TUPLE(ALL_SYNAPSES);

/**
 * @brief Structure for the parameters shared between synapses for STDP.
 * @tparam Rule type of the STDP rule.
 * @tparam SynapseType type of synapses.
 */
template <template <typename> typename Rule, typename SynapseType>
struct shared_synapse_parameters<knp::synapse_traits::STDP<Rule, SynapseType>>
{
    enum class ProcessingType
    {
        STDPOnly,
        STDPAndSpike
    };

    using ContainerType = std::unordered_map<core::UID, ProcessingType, core::uid_hash>;

    // cppcheck-suppress unusedStructMember
    uint32_t stdp_window_size_ = 1;
    ContainerType stdp_populations_;
};

}  // namespace knp::synapse_traits
