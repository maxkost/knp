/**
 * @file output_channel_utility.h
 * @brief Auxiliary functions for OutputChannel bindings.
 * @kaspersky_developer Vartenkov A.
 * @date 05.06.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <knp/framework/io/output_channel.h>

#include <memory>
#include <utility>


std::shared_ptr<knp::framework::io::output::OutputChannel> construct_output_channel(
    const knp::core::UID &uid, knp::core::MessageEndpoint &endpoint)
{
    return std::make_shared<knp::framework::io::output::OutputChannel>(uid, std::move(endpoint));
}
