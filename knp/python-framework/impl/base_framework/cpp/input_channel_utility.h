/**
 * @file input_channel_utility.h
 * @brief Auxiliary functions for input channel bindings.
 * @author Vartenkov A.
 * @date 05.06.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */


#pragma once
#include <knp/framework/io/input_channel.h>

#include <memory>
#include <utility>


std::shared_ptr<knp::framework::io::input::InputChannel> construct_input_channel(
    const knp::core::UID &uid, knp::core::MessageEndpoint &endpoint, knp::framework::io::input::DataGenerator &gen)
{
    return std::make_shared<knp::framework::io::input::InputChannel>(uid, std::move(endpoint), std::move(gen));
}


knp::core::UID get_input_channel_uid(knp::framework::io::input::InputChannel &self)
{
    return self.get_uid();
}
