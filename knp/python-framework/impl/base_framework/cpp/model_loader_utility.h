
#pragma once

#include <knp/framework/model_loader.h>

#include <memory>
#include <utility>


knp::framework::ModelLoader make_model_loader(
    std::shared_ptr<knp::core::Backend> backend, knp::framework::ModelLoader::InputChannelMap i_map)
{
    return knp::framework::ModelLoader{backend, std::move(i_map)};
}


auto &get_model_loader_output_channel(knp::framework::ModelLoader &model_loader, const knp::core::UID &uid)
{
    return model_loader.get_output_channel(uid);
}


auto &get_model_loader_input_channel(knp::framework::ModelLoader &model_loader, const knp::core::UID &uid)
{
    return model_loader.get_input_channel(uid);
}
