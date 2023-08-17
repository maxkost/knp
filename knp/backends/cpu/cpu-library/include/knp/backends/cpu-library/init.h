/**
 * @file init.h
 * @brief Backends initializing routines.
 * @author Artiom N.
 * @date 17.08.2023
 */

#pragma once

#include <knp/core/message_endpoint.h>
#include <knp/core/messaging/messaging.h>
#include <knp/core/projection.h>

#include <tuple>
#include <variant>


namespace knp::backends::cpu
{

template <typename T>
void subscribe_stdp_projection(const typename core::Projection<T> &, knp::core::MessageEndpoint &)
{
}


template <template <typename> typename Rule, typename SynapseT>
void subscribe_stdp_projection(
    const typename core::Projection<SynapseT> &p, knp::core::MessageEndpoint &message_endpoint)
{
    for (const auto &[pop_uid, _] : p.get_common_parameters().stdp_populations_)
    {
        if (pop_uid) message_endpoint.subscribe<knp::core::messaging::SpikeMessage>(p.get_uid(), {pop_uid});
    }
}


template <typename ProjectionContainer>
void init(const ProjectionContainer &projections, knp::core::MessageEndpoint &message_endpoint)
{
    for (const auto &p : projections)
    {
        const auto [pre_uid, post_uid, this_uid] = std::visit(
            [&message_endpoint](const auto &proj)
            {
                using T = std::decay_t<decltype(proj)>;
                subscribe_stdp_projection<typename T::ProjectionSynapseType>(proj, message_endpoint);
                return std::make_tuple(proj.get_presynaptic(), proj.get_postsynaptic(), proj.get_uid());
            },
            p.arg_);

        if (pre_uid) message_endpoint.subscribe<knp::core::messaging::SpikeMessage>(this_uid, {pre_uid});
        if (post_uid) message_endpoint.subscribe<knp::core::messaging::SynapticImpactMessage>(post_uid, {this_uid});
    }
}
}  // namespace knp::backends::cpu
