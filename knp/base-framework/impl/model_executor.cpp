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

template <typename GenType>
void ModelExecutor::init_channels(
    const std::unordered_multimap<core::UID, core::UID, core::uid_hash> &channels, GenType channel_gen)
{
    for (size_t i = 0; i < channels.bucket_count(); ++i)
    {
        auto bucket_iter = channels.begin(i);
        if (channels.end(i) == bucket_iter) continue;
        auto channel_uid = bucket_iter->first;

        std::vector<core::UID> p_uids(channels.bucket_size(i));
        std::transform(
            bucket_iter, channels.end(i), std::back_inserter(p_uids), [](const auto &it) { return it.second; });

        (this->*channel_gen)(channel_uid, p_uids);
    }
}


void ModelExecutor::gen_input_channel(const core::UID &channel_uid, const std::vector<core::UID> &p_uids)
{
    auto channel = i_gen_(channel_uid, backend_->message_bus_.create_endpoint());
    in_channels_.push_back(std::move(channel));
    for (const auto &u : p_uids)
    {
        backend_->get_message_endpoint().subscribe<knp::core::messaging::SpikeMessage>(u, {channel_uid});
    }
}


void ModelExecutor::gen_output_channel(const core::UID &channel_uid, const std::vector<core::UID> &p_uids)
{
    auto endpoint = backend_->message_bus_.create_endpoint();
    endpoint.subscribe<knp::core::messaging::SpikeMessage>(channel_uid, p_uids);
    auto channel = out_gen_(channel_uid, std::move(endpoint));
    out_channels_.push_back(std::move(channel));
}


void ModelExecutor::init()
{
    const auto &network = model_.get_network();

    backend_->load_all_populations(network.get_populations());
    backend_->load_all_projections(network.get_projections());

    // Create input.
    init_channels(model_.get_input_channels(), &ModelExecutor::gen_input_channel);

    // Create output.
    init_channels(model_.get_output_channels(), &ModelExecutor::gen_output_channel);
}


input::InputChannel *ModelExecutor::get_input_channel(const core::UID &channel_uid)
{
    auto result = std::find_if(
        in_channels_.cbegin(), in_channels_.cend(),
        [&channel_uid](const auto &ic) { return ic->get_uid() == channel_uid; });
    if (in_channels_.end() == result) throw std::runtime_error("Wrong input channel UID");
    return result->get();
}


output::OutputChannel *ModelExecutor::get_output_channel(const core::UID &channel_uid)
{
    auto result = std::find_if(
        out_channels_.cbegin(), out_channels_.cend(),
        [&channel_uid](const auto &oc) { return oc->get_uid() == channel_uid; });
    if (out_channels_.end() == result) throw std::runtime_error("Wrong output channel UID");
    return result->get();
}


void ModelExecutor::start()
{
    backend_->start(
        [this](knp::core::messaging::Step step)
        {
            for (auto &i_ch : in_channels_)
            {
                i_ch->send(step);
            }
            return true;
        },
        [this](knp::core::messaging::Step)
        {
            for (auto &o_ch : out_channels_)
            {
                o_ch->update();
            }

            return true;
        });
}


void ModelExecutor::start(core::Backend::RunPredicate run_predicate)
{
    backend_->start(
        [this, run_predicate](knp::core::messaging::Step step)
        {
            for (auto &i_ch : in_channels_)
            {
                i_ch->send(step);
            }
            return run_predicate(step);
        },
        [this](knp::core::messaging::Step)
        {
            for (auto &o_ch : out_channels_)
            {
                o_ch->update();
            }

            return true;
        });
}


void ModelExecutor::stop()
{
    backend_->stop();
}

}  // namespace knp::framework
