/**
 * @file observer_utility.h
 * @brief Auxiliary functions for MessageObserver bindings.
 * @author Vartenkov A.
 * @date 29.10.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/core/messaging/messaging.h>
#include <knp/framework/monitoring/observer.h>

#include <memory>
#include <utility>


template <class Message>
auto make_observer(
    knp::core::MessageEndpoint &endpoint, knp::framework::monitoring::MessageProcessor<Message> &processor,
    const knp::core::UID &uid)
{
    return knp::framework::monitoring::MessageObserver<Message>{std::move(endpoint), std::move(processor), uid};
}


#define INSTANCE_MAKE_OBSERVERS(n, template_for_instance, message_type)                               \
    template auto make_observer<knp::core::messaging::message_type>(                                  \
        knp::core::MessageEndpoint & endpoint,                                                        \
        knp::framework::monitoring::MessageProcessor<knp::core::messaging::message_type> & processor, \
        const knp::core::UID &uid);


BOOST_PP_SEQ_FOR_EACH(INSTANCE_MAKE_OBSERVERS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_MESSAGES))
