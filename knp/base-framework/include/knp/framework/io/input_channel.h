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


namespace knp::framework::input
{
/**
 * @brief Input channel class. It provides an object it's connected to with spikes.
 * @details a user would need to create a channel, associate it with a stream then provide stream with data.
 * The channel should be constructed, then connected with "connect(target_uid)", then call send() when a message
 * should be sent.
 */
class InputChannel
{
public:
    /**
     * @brief Functor used for converting stream data to spikes.
     */
    using DataConverter = std::function<core::messaging::SpikeData(std::istream &)>;

    /**
     * @brief Channel constructor.
     * @param stream stream to send data to.
     * @param converter functor that generate spike indices from stream data.
     * @param channel_uid sender UID for message headers.
     * @param endpoint endpoint used for sending messages.
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
     * @brief Get channel UID.
     * @return channel UID.
     */
    [[nodiscard]] const core::UID &get_uid() const { return uid_; }

    /**
     * @brief Get stream.
     */
    std::istream &get_stream() { return *stream_; }

    /**
     * @brief Read data from stream, form message and send it to endpoint.
     * @param time current step.
     * @return true if message was sent, false if no message sent.
     * @note throws exceptions if stream is set to throw.
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
 * @brief Connect input channel to a target object.
 * @param channel input channel.
 * @param target_endpoint receiving endpoint.
 * @param receiver_uid object UID.
 * @note target object should be able to receive spikes.
 */
inline void connect_input(
    const InputChannel &channel, core::MessageEndpoint &target_endpoint, const core::UID &receiver_uid)
{
    target_endpoint.subscribe<core::messaging::SpikeMessage>(receiver_uid, {channel.get_uid()});
}

}  // namespace knp::framework::input
