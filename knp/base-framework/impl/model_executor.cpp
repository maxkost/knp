/**
 * @file model_executor.cpp
 * @brief Model executor class implementation.
 * @author Artiom N.
 * @date 21.04.2023
 */

#include <knp/framework/model_executor.h>

#include <spdlog/spdlog.h>


namespace knp::framework
{

void ModelExecutor::start()
{
    start([](knp::core::Step) { return true; });
}


void ModelExecutor::start(core::Backend::RunPredicate run_predicate)
{
    SPDLOG_INFO("Starting model execution...");

    get_backend()->start(
        [this, run_predicate](knp::core::Step step)
        {
            // Sending inputs from the channels.
            for (auto &i_ch : loader_.get_inputs())
            {
                i_ch.send(step);
            }
            // Run user predicate.
            return run_predicate(step);
        },
        [this](knp::core::Step)
        {
            // Loading spikes into output channels.
            for (auto &o_ch : loader_.get_outputs())
            {
                o_ch.update();
            }
            // Run monitoring observers.
            for (auto &observer : observers_)
            {
                std::visit([](auto &entity) { entity.update(); }, observer);
            }

            return true;
        });
    SPDLOG_INFO("Model execution stopped...");
}


void ModelExecutor::stop()
{
    get_backend()->stop();
}

}  // namespace knp::framework
