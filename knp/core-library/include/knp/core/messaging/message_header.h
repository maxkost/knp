/**
 * @file message_header.h
 * @brief Message headers class.
 * @author Artiom N.
 * @date 26.01.2023
 */

#pragma once

#include <knp/core/uid.h>


namespace knp::core::messaging
{

/**
 * @brief Common message headers.
 */
struct MessageHeader
{
    UID sender_uid_;
    time_t send_time_;
};

}  // namespace knp::core::messaging
