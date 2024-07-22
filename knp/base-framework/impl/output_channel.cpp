/**
 * @file output_channel.cpp
 * @brief Output channel class.
 * @author Vartenkov Andrey.
 * @date 01.06.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/framework/io/output_channel.h>

namespace knp::framework::io::output
{
auto comp_lower(const core::messaging::SpikeMessage &message, core::Step step)
{
    return message.header_.send_time_ < step;
}


auto comp_upper(core::Step step, const core::messaging::SpikeMessage &message)
{
    return step < message.header_.send_time_;
}


std::vector<core::messaging::SpikeMessage> OutputChannel::update()
{
    endpoint_.receive_all_messages();
    auto messages = endpoint_.unload_messages<core::messaging::SpikeMessage>(base_.uid_);

    message_buffer_.reserve(message_buffer_.size() + messages.size());
    for (auto &&message : messages)
    {
        // cppcheck-suppress useStlAlgorithm
        message_buffer_.push_back(std::move(message));
    }

    return message_buffer_;
}


std::vector<core::messaging::SpikeMessage> OutputChannel::read_some_from_buffer(
    core::Step starting_step, core::Step final_step)
{
    std::vector<core::messaging::SpikeMessage> result;

    result.reserve(starting_step + final_step + 1);
    // Here we suppose that buffer is sorted by sending step as it should be
    auto begin_iter = std::lower_bound(message_buffer_.begin(), message_buffer_.end(), starting_step, comp_lower);
    auto end_iter = std::upper_bound(message_buffer_.begin(), message_buffer_.end(), final_step, comp_upper);

    std::move(begin_iter, end_iter, std::back_inserter(result));
    message_buffer_.erase(begin_iter, end_iter);
    return result;
}

}  // namespace knp::framework::io::output
