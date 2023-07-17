/**
 * @file model_executor.cpp
 * @brief Model executor class implementation.
 * @author Artiom N.
 * @date 21.04.2023
 */

#include <knp/framework/model_executor.h>

#include <spdlog/spdlog.h>

#define _KNP_INTERNAL

namespace knp::framework
{

void ModelExecutor::init()
{
    const auto &network = model_.get_network();

    backend_->load_all_populations(network.get_populations());
    backend_->load_all_projections(network.get_projections());

    // Create input.
    for (auto [input_ch, proj_uid] : model_.get_input_channels())
    {
        backend_->get_message_endpoint().subscribe<knp::core::messaging::SpikeMessage>(proj_uid, {input_ch->get_uid()});
    }

    // Create output.
    for (auto [output_ch, pop_uid] : model_.get_input_channels())
    {
        backend_->get_message_endpoint().subscribe<knp::core::messaging::SpikeMessage>(output_ch->get_uid(), {pop_uid});
    }
}


void ModelExecutor::start()
{
    backend_->start();
}


void ModelExecutor::stop()
{
    backend_->stop();
}

}  // namespace knp::framework
