/**
 * @brief Input channel header
 * @author Vartenkov Andrey
 * @date 04.05.2023
 */
#pragma once

#include <knp/core/input_converter.h>

#include <memory>
#include <utility>

namespace knp::core::input
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
    using DataConverter = std::function<messaging::SpikeData(std::istream &, size_t)>;

    /**
     * @brief Channel constructor.
     * @param stream stream to send data to.
     * @param converter functor that generate spike indices from stream data.
     * @param channel_uid sender UID for message headers.
     * @param endpoint endpoint used for sending messages.
     * @param size input size.
     */
    InputChannel(
        MessageEndpoint &endpoint, std::unique_ptr<std::istream> &&stream, DataConverter converter, UID channel_uid,
        size_t size)
        : endpoint_(endpoint),
          stream_(std::move(stream)),
          converter_(std::move(converter)),
          uid_(channel_uid),
          size_(size)
    {
    }

    /**
     * @brief Connect channel to a target object.
     * @param receiver_uid object UID.
     * @note target object should be able to receive spikes.
     */
    void connect(const UID &receiver_uid) { endpoint_.subscribe<messaging::SpikeMessage>(receiver_uid, {uid_}); }

    /**
     * @brief Change the channel data converter.
     * @param converter
     */
    void set_converter(const DataConverter &converter) { converter_ = converter; }

    /**
     * @brief Sets a different input size for a channel.
     * @param new_size new input size value.
     * @note size value is a converter parameter, it might have different meanings for different converters.
     */
    void set_size(size_t new_size) { size_ = new_size; }

    /**
     * @brief Set a new UID to the channel. Can be used to switch between different receivers.
     * @param new_uid new sender UID.
     */
    void set_uid(UID new_uid) { uid_ = new_uid; }

    /**
     * @brief Get channel UID.
     * @return channel UID.
     */
    UID get_uid() { return uid_; }

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
    bool send(messaging::TimeType time)
    {
        messaging::SpikeData spikes = converter_(*stream_, size_);
        if (spikes.empty()) return false;

        messaging::SpikeMessage message{{uid_, time}, std::move(spikes)};
        endpoint_.send_message(message);
        return true;
    }

private:
    /**
     * @brief A reference to the endpoint used by channel to send messages.
     */
    MessageEndpoint &endpoint_;

    /**
     * @brief A reference to the stream from which to read data.
     */
    std::unique_ptr<std::istream> stream_;

    /**
     * @brief Converter functor: gets stream and input size, returns a list of spikes.
     */
    DataConverter converter_;

    /**
     * @brief Channel own UID, used as a sender UID for messages.
     */
    UID uid_;

    /**
     * @brief Input size, as used by converter.
     */
    size_t size_;
};

}  // namespace knp::core::input
