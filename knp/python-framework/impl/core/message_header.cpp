/**
 * @file message_header.cpp
 * @brief Message header Python bindings.
 * @author Artiom N.
 * @date 01.02.2024
 */
#include "common.h"


#if defined(_KNP_IN_CORE)

py::class_<core::messaging::MessageHeader>("MessageHeader", "Common header for messages.");

#endif
