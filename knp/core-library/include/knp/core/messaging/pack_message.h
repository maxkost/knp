/**
 * @file pack_message.h
 * @brief Message packing and unpacking routines.
 * @author Artiom N.
 * @date 13.04.2023
 */

#pragma once

#include <vector>


namespace knp::core::messaging
{

template <typename T>
T unpack(std::vector<uint8_t>& buffer);

template <typename T>
T unpack(const void* buffer);

}  // namespace knp::core::messaging
