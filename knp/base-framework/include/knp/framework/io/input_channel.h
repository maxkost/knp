/**
 * @brief Input channel header
 * @author Vartenkov Andrey
 * @date 04.05.2023
 */
#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "input_converter.h"


/**
 * @brief Input channel namespace.
 */
namespace knp::framework::input
{
/**
 * @brief The InputChannel defines an input channel that provides a connected entity (for example, an input projection)
 * with spike messages based on the input stream data.
 * @details You need to create an input channel, associate it with an input stream and provide the stream with input
 * data. After constructing an input channel, connect it with an input entity (for example, a projection) using the
 * `connect_input(channel, target_endpoint, receiver_uid)` method. To send the input data to a connected entity, call
 * the `send(time)` method.
 */
class InputChannel
{
public:
    /**
     * @brief Input channel constructor.
     * @param stream stream from which to receive data.
     * @param converter functor that generates spike messages based on data from the input stream.
     * @param channel_uid sender UID to put into the message header.
     * @param endpoint endpoint used to send messages.
     */
    InputChannel(
        std::unique_ptr<std::istream> &&stream, core::MessageEndpoint &&endpoint, DataConverter converter,
        core::UID channel_uid = core::UID{true})
        : stream_(std::move(stream)),
          endpoint_(std::move(endpoint)),
          converter_(std::move(converter)),
          uid_(channel_uid)
    {
    }

    /**
     * @brief Get input channel UID.
     * @return input channel UID.
     */
    [[nodiscard]] const core::UID &get_uid() const { return uid_; }

    /**
     * @brief Get input stream.
     */
    std::istream &get_stream() { return *stream_; }

    /**
     * @brief Read data from input stream, form a spike message and send it to an endpoint.
     * @note The method throws exceptions if an input stream is set to throw exceptions.
     * @param time current step.
     * @return true if message was sent, false if no message was sent.
     */
    bool send(core::messaging::Step time)
    {
        core::messaging::SpikeData spikes = converter_(*stream_);
        if (spikes.empty()) return false;

        core::messaging::SpikeMessage message{{uid_, time}, std::move(spikes)};
        endpoint_.send_message(message);
        return true;
    }

private:
    /**
     * @brief A reference to the stream from which to read data.
     */
    std::unique_ptr<std::istream> stream_;

    /**
     * @brief A reference to the endpoint used by channel to send messages.
     */
    core::MessageEndpoint endpoint_;

    /**
     * @brief Converter functor: gets stream, returns a list of spikes.
     */
    DataConverter converter_;

    /**
     * @brief Channel own UID, used as a sender UID for messages.
     */
    const core::UID uid_;
};

/**
 * @brief Connect input channel to a target entity.
 * @note Target entity should be able to receive spikes.
 * @param channel input channel.
 * @param target_endpoint endpoint to use for message exchange.
 * @param receiver_uid UID of an entity that receives spike messages.
 */
inline void connect_input(
    const InputChannel &channel, core::MessageEndpoint &target_endpoint, const core::UID &receiver_uid)
{
    target_endpoint.subscribe<core::messaging::SpikeMessage>(receiver_uid, {channel.get_uid()});
}

}  // namespace knp::framework::input
