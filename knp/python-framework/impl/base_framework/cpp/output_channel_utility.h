//
// Created by an_vartenkov on 14.06.24.
//

#pragma once

#include <knp/framework/io/output_channel.h>

#include <memory>
#include <utility>


std::shared_ptr<knp::framework::output::OutputChannel> construct_output_channel(
    const knp::core::UID &uid, knp::core::MessageEndpoint &endpoint)
{
    return std::make_shared<knp::framework::output::OutputChannel>(uid, std::move(endpoint));
}
