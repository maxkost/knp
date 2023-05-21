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

void ModelExecutor::run()
{
    const auto &network = model_.get_network();

    for (const auto &population = network.begin_populations(); population != network.end_populations();
         /*++population*/)
    {
        // backend_->
    }
}

}  // namespace knp::framework
