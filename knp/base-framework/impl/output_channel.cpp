/**
 * @file output_channel.cpp
 * @brief Output channel class.
 * @author Vartenkov Andrey.
 * @date 01.06.2023
 */

#include <knp/framework/io/output_channel.h>

namespace knp::framework::output
{

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
    core::messaging::Step starting_step, core::messaging::Step final_step)
{
    std::vector<core::messaging::SpikeMessage> result;
    result.reserve(starting_step + final_step + 1);
    // Here we suppose that buffer is sorted by sending step as it should be
    auto comp_lower = [](const core::messaging::SpikeMessage &message, core::messaging::Step step)
    { return message.header_.send_time_ < step; };
    auto begin_iter = std::lower_bound(message_buffer_.begin(), message_buffer_.end(), starting_step, comp_lower);

    auto comp_upper = [](core::messaging::Step step, const core::messaging::SpikeMessage &message)
    { return step < message.header_.send_time_; };
    auto end_iter = std::upper_bound(message_buffer_.begin(), message_buffer_.end(), final_step, comp_upper);

    std::move(begin_iter, end_iter, std::back_inserter(result));
    message_buffer_.erase(begin_iter, end_iter);
    return result;
}

}  // namespace knp::framework::output
