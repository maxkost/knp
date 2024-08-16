//
// Created by an_vartenkov on 14.06.24.
//

#pragma once
#include <knp/framework/io/input_channel.h>

#include <memory>
#include <utility>


std::shared_ptr<knp::framework::input::InputChannel> construct_input_channel(
    const knp::core::UID &uid, knp::core::MessageEndpoint &endpoint, knp::framework::input::DataGenerator &gen)
{
    return std::make_shared<knp::framework::input::InputChannel>(uid, std::move(endpoint), std::move(gen));
}


knp::core::UID get_input_channel_uid(knp::framework::input::InputChannel &self)
{
    return self.get_uid();
}
