/**
 * @file model.cpp
 * @brief Model class implementation.
 * @author Artiom N.
 * @date 30.03.2023
 */

#include <knp/framework/model.h>

#include <spdlog/spdlog.h>

#define _KNP_INTERNAL

namespace knp::framework
{

core::UID Model::add_input_channel(
    std::unique_ptr<knp::framework::input::InputChannel> &&channel, const core::UID &projection_uid)
{
    core::UID uid = channel->get_uid();
    in_channels_.insert(std::make_pair(uid, std::make_tuple(std::move(channel), projection_uid)));
    return uid;
}


core::UID Model::add_output_channel(
    std::unique_ptr<knp::framework::output::OutputChannelBase> &&channel_ptr, const core::UID &population_uid)
{
    core::UID uid = channel_ptr->get_uid();
    out_channels_.insert(std::make_pair(uid, std::make_tuple(std::move(channel_ptr), population_uid)));
    return uid;
}


output::OutputChannelBase *Model::get_output_channel(const core::UID &channel_uid)
{
    auto result = out_channels_.find(channel_uid);
    if (result == out_channels_.end() || !std::get<0>(result->second))
        throw std::runtime_error("Wrong output channel UID");
    return std::get<0>(result->second).get();
}


input::InputChannel *Model::get_input_channel(const core::UID &channel_uid)
{
    auto result = in_channels_.find(channel_uid);
    if (result == in_channels_.end()) throw std::runtime_error("Wrong input channel UID");
    return std::get<0>(result->second).get();
}


std::vector<std::tuple<input::InputChannel *, core::UID>> Model::get_input_channels() const
{
    std::vector<std::tuple<input::InputChannel *, core::UID>> result;

    result.reserve(in_channels_.size());

    for (const auto &ic : in_channels_)
        result.push_back(std::make_tuple(std::get<0>(ic.second).get(), std::get<1>(ic.second)));

    return result;
}


std::vector<std::tuple<output::OutputChannelBase *, core::UID>> Model::get_output_channels() const
{
    std::vector<std::tuple<output::OutputChannelBase *, core::UID>> result;

    result.reserve(out_channels_.size());

    for (auto &ic : out_channels_)
        result.push_back(std::make_tuple(std::get<0>(ic.second).get(), std::get<1>(ic.second)));

    return result;
}

}  // namespace knp::framework
