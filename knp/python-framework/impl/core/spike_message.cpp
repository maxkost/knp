/**
 * @file spike_message.cpp
 * @brief SpikeMessage Python bindings.
 * @author Artiom N.
 * @date 01.02.2024
 */
#include "common.h"


#if defined(_KNP_IN_CORE)

py::class_<core::messaging::SpikeMessage>("SpikeMessage", "Structure of the spike message.");

#endif
