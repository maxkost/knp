/**
 * @file subscription.cpp
 * @brief Instantiation of subscriptions.
 * @author Artiom N.
 * @date 11.04.2023
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

#include <knp/core/messaging/messaging.h>
#include <knp/core/subscription.h>

#include <boost/preprocessor.hpp>

namespace knp::core
{

#define INSTANCE_SUBSCRIPTIONS(n, template_for_instance, message_type) \
    template class knp::core::Subscription<knp::core::messaging::message_type>;

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_SUBSCRIPTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_MESSAGES))

}  // namespace knp::core
