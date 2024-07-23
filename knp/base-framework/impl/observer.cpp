/**
 * @file observer.cpp
 * @brief Observers implementation.
 * @author Artiom N.
 * @date 26.02.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/framework/monitoring/observer.h>

#include <boost/preprocessor.hpp>

// #define _KNP_INTERNAL

namespace knp::framework::monitoring
{

#define INSTANCE_OBSERVERS(n, template_for_instance, message_type) \
    template class MessageObserver<knp::core::messaging::message_type>;

BOOST_PP_SEQ_FOR_EACH(INSTANCE_OBSERVERS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_MESSAGES))

}  // namespace knp::framework::monitoring
