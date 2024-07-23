/**
 * @file spike_message.h
 * @brief SpikeMessage Python bindings header.
 * @author Artiom N.
 * @date 28.02.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <memory>
#include <utility>

#include "common.h"


std::shared_ptr<knp::core::messaging::SpikeMessage> spike_message_constructor(
    const py::tuple& header, const py::list& spikes)
{
    core::messaging::SpikeMessage sm = {
        {py::extract<core::UID>(header[0]), py::extract<size_t>(header[1])},
        py_iterable_to_vector<core::messaging::SpikeIndex>(spikes)};

    return std::make_shared<knp::core::messaging::SpikeMessage>(std::move(sm));
}
