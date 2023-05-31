/**
 * @brief Output channel header
 * @author Vartenkov Andrey
 * @date 11.05.2023
 */
#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#include "knp/core/message_endpoint.h"
#include "knp/core/messaging/messaging.h"
#include "output_converter.h"


/**
 * @brief Network output processing namespace.
 */
namespace knp::framework::output
{
/**
 * @brief Base class for output channels.
 */
class OutputChannelBase
{
public:
    /**
     * Base channel constructor.
     * @param channel_uid channel UID.
     * @param endpoint the endpoint channel uses for message exchange. Not owned.
     * @param sender_uid UID of the sender entity.
     */
    OutputChannelBase(const core::UID &channel_uid, core::MessageEndpoint &endpoint, core::UID sender_uid)
        : uid_{channel_uid}, endpoint_(endpoint)
    {
        endpoint_.subscribe<core::messaging::SpikeMessage>(uid_, {sender_uid});
    }

    /**
     * Base channel constructor.
     * @param endpoint the endpoint channel uses for message exchange. Not owned.
     * @param sender_uid UID of the sender entity.
     */
    OutputChannelBase(core::MessageEndpoint &endpoint, core::UID sender_uid) : uid_{true}, endpoint_(endpoint)
    {
        endpoint_.subscribe<core::messaging::SpikeMessage>(uid_, {sender_uid});
    }

    /**
     * @brief Get channel UID.
     * @return channel UID.
     */
    [[nodiscard]] const core::UID &get_uid() const { return uid_; }

protected:
    /**
     * @brief Channel UID.
     */
    core::UID uid_;

    /**
     * @brief Endpoint reference for message processing.
     */
    core::MessageEndpoint &endpoint_;

    /**
     * @brief Messages the channel has got from
     */
    std::vector<core::messaging::SpikeMessage> message_buffer_;
};


/**
 * @brief Output channel class that uses stream interface.
 * @tparam ResultType type of return values
 */
template <class ResultType>
class OutputChannel : public OutputChannelBase
{
public:
    /**
     * @brief Output channel constructor.
     * @param endpoint endpoint for message exchange.
     * @param converter data converter.
     * @param sender_uid UID of the sender population.
     */
    OutputChannel(core::MessageEndpoint &endpoint, OutputConverter<ResultType> converter, core::UID sender_uid)
        : OutputChannelBase(endpoint, sender_uid), converter_(std::move(converter))
    {
    }

    /**
     * @brief Output channel constructor.
     * @param channel_uid channel UID.
     * @param endpoint endpoint for message exchange.
     * @param converter data converter.
     * @param sender_uid UID of the sender population.
     */
    OutputChannel(
        const core::UID &channel_uid, core::MessageEndpoint &endpoint, OutputConverter<ResultType> converter,
        core::UID sender_uid)
        : OutputChannelBase(channel_uid, endpoint, sender_uid), converter_(std::move(converter))
    {
    }

    /**
     * @brief Read all accumulated messages from subscription, then convert them to data.
     * @param step_from starting step.
     * @param step_to final step for messages. Messages send at this step are also included.
     * @return data in required format.
     */
    [[nodiscard]] ResultType get(core::messaging::Step step_from, core::messaging::Step step_to)
    {
        update();
        return converter_(read_some_from_buffer(step_from, step_to));
    }

private:
    /**
     * @brief Data converter function.
     */
    OutputConverter<ResultType> converter_;

    /**
     * @brief updates message buffer. Should be done before reading data from the channel.
     */
    void update()
    {
        auto messages = endpoint_.unload_messages<core::messaging::SpikeMessage>(uid_);

        message_buffer_.reserve(message_buffer_.size() + messages.size());
        for (auto &&message : messages)
        {
            message_buffer_.push_back(std::move(message));
        }
    }

    /**
     * @brief Read a (closed) interval of messages from sorted internal buffer.
     * @param starting_step left interval border.
     * @param final_step right interval border. Messages sent at this step are also included.
     * @return a vector of messages with send_time values in [starting_step, final_step].
     */
    std::vector<core::messaging::SpikeMessage> read_some_from_buffer(
        core::messaging::Step starting_step, core::messaging::Step final_step)
    {
        std::vector<core::messaging::SpikeMessage> result;
        result.reserve(starting_step + final_step + 1);
        // Here we suppose that buffer is sorted by sending step as it should be
        auto begin_iter = message_buffer_.begin();
        while (begin_iter < message_buffer_.end() && begin_iter->header_.send_time_ < starting_step)
        {
            ++begin_iter;
        }
        auto end_iter = begin_iter;
        while (end_iter < message_buffer_.end() && end_iter->header_.send_time_ <= final_step)
        {
            ++end_iter;
        }
        // Between begin_iter and final_iter there are the messages we need. Move them to result then remove from
        // buffer.
        std::move(begin_iter, end_iter, std::back_inserter(result));
        message_buffer_.erase(begin_iter, end_iter);
        return result;
    }
};


// TODO: Add instantiation for most frequently used result types: int, float, vector<uint>, vector<bool>, mb set<uint>.

}  // namespace knp::framework::output
