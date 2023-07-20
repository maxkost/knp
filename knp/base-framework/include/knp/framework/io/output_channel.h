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
 * @brief Output channel namespace.
 */
namespace knp::framework::output
{
/**
 * @brief The OutputChannelBase class is a base class for output channels.
 */
class OutputChannelBase
{
public:
    /**
     * @brief Base output channel constructor.
     * @param channel_uid output channel UID.
     * @param endpoint endpoint to use for message exchange.
     * @param sender_uid UID of the sender entity.
     */
    OutputChannelBase(const core::UID &channel_uid, core::MessageEndpoint &endpoint, core::UID sender_uid)
        : uid_{channel_uid}, endpoint_(endpoint)
    {
        endpoint_.subscribe<core::messaging::SpikeMessage>(uid_, {sender_uid});
    }

    /**
     * @brief Base output channel constructor.
     * @param endpoint endpoint to use for message exchange.
     * @param sender_uid UID of the sender entity.
     */
    OutputChannelBase(core::MessageEndpoint &endpoint, core::UID sender_uid) : uid_{true}, endpoint_(endpoint)
    {
        endpoint_.subscribe<core::messaging::SpikeMessage>(uid_, {sender_uid});
    }

    /**
     * @brief Get output channel UID.
     * @return output channel UID.
     */
    [[nodiscard]] const core::UID &get_uid() const { return uid_; }

protected:
    /**
     * @brief Output channel UID.
     */
    core::UID uid_;

    /**
     * @brief Reference to an endpoint used for message exchange.
     */
    core::MessageEndpoint &endpoint_;

    /**
     * @brief Messages received from output population.
     */
    std::vector<core::messaging::SpikeMessage> message_buffer_;
};


/**
 * @brief The OutputChannel class is a definition of an interface to output channels.
 * @tparam ResultType output data type.
 */
template <class ResultType>
class OutputChannel : public OutputChannelBase
{
public:
    /**
     * @brief Output channel constructor.
     * @param endpoint endpoint to use for message exchange.
     * @param converter data converter.
     * @param sender_uid UID of the population that sends spike messages.
     */
    OutputChannel(core::MessageEndpoint &endpoint, OutputConverter<ResultType> converter, core::UID sender_uid)
        : OutputChannelBase(endpoint, sender_uid), converter_(std::move(converter))
    {
    }

    /**
     * @brief Output channel constructor.
     * @param channel_uid output channel UID.
     * @param endpoint endpoint to use for message exchange.
     * @param converter data converter.
     * @param sender_uid UID of the population that sends spike messages.
     */
    OutputChannel(
        const core::UID &channel_uid, core::MessageEndpoint &endpoint, OutputConverter<ResultType> converter,
        core::UID sender_uid)
        : OutputChannelBase(channel_uid, endpoint, sender_uid), converter_(std::move(converter))
    {
    }

    /**
     * @brief Read all accumulated spike messages from subscription and convert them to output data.
     * @param step_from network step from which the method starts reading spike messages.
     * @param step_to network step after which the method stops reading spike messages.
     * @return output data in the required format.
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
     * @brief Unload spike messages from the endpoint into the message buffer. 
     * @details You should call the method before reading data from the channel.
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
     * @brief Read a specified interval of messages from sorted internal message buffer.
     * @param starting_step step from which the method starts reading spike messages.
     * @param final_step step after which the method stops reading spike messages.
     * @return vector of messages sent on the specified interval of steps.
     */
    std::vector<core::messaging::SpikeMessage> read_some_from_buffer(
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
};

}  // namespace knp::framework::output
