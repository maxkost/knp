/**
 * @brief Input channel header
 * @author Vartenkov Andrey
 * @date 25.04.2023
 */
#pragma once

#include <knp/core/messaging/messaging.h>
#include <knp/core/uid.h>

#include <spdlog/spdlog.h>

#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

/**
 * @brief Input channels namespace.
 */
namespace knp::core::input
{
/**
 * @brief Base class for all data converters. It has a stream-like interface.
 */
class DataConverter
{
public:
    /**
     * @brief Constructor for the base class, initialize input size.
     * @param input_size expected size of an input projection.
     */
    explicit DataConverter(size_t input_size) : input_size_(input_size) {}

    /**
     * @brief Default virtual destructor.
     */
    virtual ~DataConverter() = default;

    /**
     * @brief output stream operator. It allows user to get messages from the stream.
     * @param out_message the resulting spike message. You should initialize sending time out of this function.
     * @return reference to the stream.
     */
    virtual DataConverter &operator>>(messaging::SpikeMessage &out_message) = 0;

    /**
     * @brief Update expected target projection size.
     * @param new_size new size of the input projection.
     */
    void set_input_size(size_t new_size) { input_size_ = new_size; }

    /**
     * @brief Call this function to switch off non-critical error notifications.
     * @param skip true: skip errors, false: don't skip errors.
     */
    void set_skip_errors(bool skip = true) { skip_errors_ = skip; }

protected:
    /**
     * @brief expected target projection size.
     */
    size_t input_size_;
    bool skip_errors_ = false;
};


/**
 * @brief A very simple function to turn values to spike or not. Casts a value to boolean type.
 * @tparam value_type input value type. Something that can be cast to bool implicitly. Usually an arithmetic type.
 * @param v input value
 * @return bool(v)
 */
template <class value_type>
bool interpret_as_bool(value_type v)
{
    return v;
}


/**
 * @brief Stream-like converter class that converts a list of values into spike messages.
 * @tparam value_type the type of values this class reads from the input stream.
 */
template <class value_type>
class SequenceConverter : public DataConverter
{
public:
    /**
     * @brief Class constructor.
     * @param input_stream stream that contains unprocessed data.
     * @param sender_uid "Sender UID" for output message header.
     * @param input_size expected target projection size.
     * @param interpret function that decides if the unprocessed value is a spike or not.
     */
    SequenceConverter(
        std::istream &input_stream, UID sender_uid, size_t input_size,
        std::function<bool(value_type)> interpret = interpret_as_bool<value_type>)
        : DataConverter(input_size), stream_(input_stream), uid_(sender_uid), interpret_(std::move(interpret))
    {
    }

    /**
     * @brief Output stream operator.
     * @param out_message output spike message. This function doesn't change its sending time, as it's not sent yet.
     * @return reference to the stream.
     */
    DataConverter &operator>>(messaging::SpikeMessage &out_message) override
    {
        SPDLOG_TRACE("Getting message from a stream using sequence converter.");
        out_message.header_.sender_uid_ = uid_;

        std::vector<uint32_t> message_data;
        for (size_t i = 0; i < input_size_; ++i)
        {
            value_type value;
            stream_ >> value;
            if (interpret_(value))
            {
                message_data.push_back(i);
            }
        }
        out_message.neuron_indexes_ = std::move(message_data);
        SPDLOG_TRACE(
            std::string("Finished loading a message, it contains ") +
            std::to_string(out_message.neuron_indexes_.size()) + " spikes.");
        return *this;
    }

private:
    /**
     * @brief Input stream for raw data.
     */
    std::istream &stream_;

    /**
     * @brief "Sender" UID for messages.
     */
    UID uid_;

    /**
     * @brief interpretation function, should return "true" for spike and "false" for no spike. Any "wrong symbol"
     * error processing logic also goes here.
     */
    std::function<bool(value_type)> interpret_;
};

}  // namespace knp::core::input
