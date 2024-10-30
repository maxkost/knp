/**
 * @file observer_utility.h
 * @brief Auxiliary functions for MessageObserver bindings.
 * @author Vartenkov A.
 * @date 29.10.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/framework/monitoring/observer.h>

#include <utility>


template <class Message>
auto make_observer(
    knp::core::MessageEndpoint &endpoint, knp::framework::monitoring::MessageProcessor<Message> &processor,
    const knp::core::UID &uid)
{
    return knp::framework::monitoring::MessageObserver<Message>{std::move(endpoint), std::move(processor), uid};
}
