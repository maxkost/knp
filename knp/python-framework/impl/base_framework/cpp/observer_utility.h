/**
 * @file observer_utility.h
 * @brief Auxiliary functions for MessageObserver bindings.
 * @kaspersky_support Vartenkov A.
 * @date 29.10.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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


template auto make_observer<knp::core::messaging::SpikeMessage>(
    knp::core::MessageEndpoint &endpoint,
    knp::framework::monitoring::MessageProcessor<knp::core::messaging::SpikeMessage> &processor,
    const knp::core::UID &uid);


template auto make_observer<knp::core::messaging::SynapticImpactMessage>(
    knp::core::MessageEndpoint &endpoint,
    knp::framework::monitoring::MessageProcessor<knp::core::messaging::SynapticImpactMessage> &processor,
    const knp::core::UID &uid);
