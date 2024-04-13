/**
 * @file init.h
 * @brief Initializing routines for backend.
 * @author Artiom N.
 * @date 17.08.2023
 */

#pragma once

#include <knp/core/message_endpoint.h>
#include <knp/core/messaging/messaging.h>
#include <knp/core/projection.h>

#include <spdlog/spdlog.h>

#include <string>
#include <tuple>
#include <variant>


/**
 * @brief Namespace for CPU backends.
 */
namespace knp::backends::cpu
{

/**
 * @brief The "null" subscriber.
 * @tparam SynapseType type of the non-STDP synapses.
 */
template <typename SynapseType>
struct subscribe_stdp_projection
{
    /**
     * @brief Subscribe STDP projection to messages.
     */
    static void subscribe(const typename core::Projection<SynapseType> &, knp::core::MessageEndpoint &) {}
};


/**
 * @brief STDP projection subscriber.
 * @tparam Rule STDP rule.
 * @tparam SynapseType linked synapse type.
 */
template <template <typename> typename Rule, typename SynapseType>
struct subscribe_stdp_projection<knp::synapse_traits::STDP<Rule, SynapseType>>
{
    /**
     * @brief Subscribe projection to message endpoint.
     * @param p projection to subscribe.
     * @param message_endpoint message endpoint to subscribe.
     */
    static void subscribe(
        const typename core::Projection<knp::synapse_traits::STDP<Rule, SynapseType>> &p,
        knp::core::MessageEndpoint &message_endpoint)
    {
        SPDLOG_TRACE("Subscribing method for the STDP projection {}", std::string(p.get_uid()));
        for (const auto &[pop_uid, _] : p.get_shared_parameters().stdp_populations_)
        {
            SPDLOG_TRACE("Subscribing STDP projection {} to {}", std::string(p.get_uid()), std::string(pop_uid));
            if (pop_uid) message_endpoint.subscribe<knp::core::messaging::SpikeMessage>(p.get_uid(), {pop_uid});
        }
    }
};


/**
 * @brief Initialize backend.
 * @param projections container backend projections.
 * @param message_endpoint message endpoint.
 * @tparam ProjectionContainer type of projection container.
 */
template <typename ProjectionContainer>
void init(const ProjectionContainer &projections, knp::core::MessageEndpoint &message_endpoint)
{
    for (const auto &p : projections)
    {
        const auto [pre_uid, post_uid, this_uid] = std::visit(
            [&message_endpoint](const auto &proj)
            {
                using T = std::decay_t<decltype(proj)>;
                subscribe_stdp_projection<typename T::ProjectionSynapseType>::subscribe(proj, message_endpoint);
                return std::make_tuple(proj.get_presynaptic(), proj.get_postsynaptic(), proj.get_uid());
            },
            p.arg_);

        if (pre_uid) message_endpoint.subscribe<knp::core::messaging::SpikeMessage>(this_uid, {pre_uid});
        if (post_uid) message_endpoint.subscribe<knp::core::messaging::SynapticImpactMessage>(post_uid, {this_uid});
    }
}

}  // namespace knp::backends::cpu
