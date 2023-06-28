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
    // TODO: UNCOMMENT THIS!
    /*
    const auto &network = model_.get_network();
    for (auto iter = network.begin_populations(); iter != network.end_populations(); ++iter)
        backend_->add_population(*iter);

    for (auto iter = network.begin_projections(); iter != network.end_projections(); ++iter)
        backend_->add_projection(*iter);

    */
    // network.get_population<>()
}

void ModelExecutor::run()
{
    // const auto &network = model_.get_network();

    is_stop_.store(false);

    while (!is_stop_)
    {
        backend_->step();
    }
}

}  // namespace knp::framework
