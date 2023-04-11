/**
 * @file subscription.cpp
 * @brief Subscriptions instantiation.
 * @author Artiom N.
 * @date 11.04.2023
 */

#include <knp/core/messaging.h>
#include <knp/core/subscription.h>

#include <boost/preprocessor.hpp>

namespace knp::core
{

#define INSTANCE_SUBSCRIPTIONS(n, template_for_instance, message_type) \
    template class knp::core::Subscription<message_type>;

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_SUBSCRIPTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_MESSAGES))

}  // namespace knp::core
