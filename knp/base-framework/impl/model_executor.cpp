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
