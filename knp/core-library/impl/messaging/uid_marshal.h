/**
 * @file uid_marshal.h
 * @brief Implementation of UID marshaling.
 * @author Artiom N.
 * @date 12.10.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once


namespace knp::core::messaging
{

inline marshal::UID get_marshaled_uid(const knp::core::UID& uid)
{
    return flatbuffers::span<const uint8_t, decltype(uid.tag)::static_size()>(uid.tag.data, uid.tag.size());
}

}  // namespace knp::core::messaging
