/**
 * @brief Input converter header
 * @author Vartenkov Andrey
 * @date 25.04.2023
 */
#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "knp/core/message_endpoint.h"
#include "knp/core/messaging/spike_message.h"
#include "knp/core/uid.h"
#include "spdlog/include/spdlog/spdlog.h"

/**
 * @brief Input channels namespace.
 */
namespace knp::framework::input
{
/**
 * @brief Stream-like converter functor class that converts a list of values into spike messages.
 * @tparam value_type the type of values this class reads from the input stream.
 * @details construction example : SequenceConverter<float> converter{interpreter_with_threshold<float>(1.0f)};
 */
template <class value_type>
class SequenceConverter
{
public:
    /**
     * @brief Class constructor.
     * @param interpret function that decides if the unprocessed value is a spike or not.
     * @param data_size size of input projection.
     */
    SequenceConverter(std::function<bool(value_type)> interpret, size_t data_size)
        : interpret_(std::move(interpret)), data_size_(data_size)
    {
    }

    /**
     * @brief Call conversion function to get indexes from stream.
     * @param stream data stream.
     * @return index vector.
     */
    core::messaging::SpikeData operator()(std::istream &stream)
    {
        SPDLOG_TRACE("Getting message from a stream using sequence converter.");

        core::messaging::SpikeData message_data;
        for (size_t i = 0; i < data_size_; ++i)
        {
            value_type value;
            stream >> value;
            if (interpret_(value))
            {
                message_data.push_back(i);
            }
        }
        SPDLOG_TRACE("Finished loading a message, it contains {} spikes", message_data.size());
        return message_data;
    }

    /**
     * Change size parameter.
     * @param size new size.
     */
    void set_size(size_t size) { data_size_ = size; }

private:
    /**
     * @brief interpretation function, should return "true" for spike and "false" for no spike. Any "wrong symbol"
     * error processing logic also goes here.
     */
    std::function<bool(value_type)> interpret_;

    /**
     * @brief input projection size.
     */
    size_t data_size_;
};


/**
 * @brief A functor class that converts lines of integers into spike indexes.
 */
class IndexConverter
{
public:
    /**
     * @brief Create converter with a delimiter.
     * @param delim a symbol that delimits spike indexes.
     */
    explicit IndexConverter(char delim = ',') : delim_(delim) {}

    /**
     * @brief Call conversion function to get indexes from stream.
     * @param stream data stream.
     * @return index vector.
     */
    core::messaging::SpikeData operator()(std::istream &stream) const
    {
        core::messaging::SpikeData result;

        std::string buffer_string;
        std::getline(stream, buffer_string);

        auto iter_first = buffer_string.begin();
        auto iter_second = iter_first;

        while (iter_first < buffer_string.end())
        {
            iter_second = std::find(iter_second + 1, buffer_string.end(), delim_);
            result.push_back(std::stoul(
                buffer_string.substr(iter_first - buffer_string.begin(), iter_second - buffer_string.begin())));
            iter_first = iter_second + 1;
        }
        return result;
    }

private:
    /**
     * @brief delimiter character.
     */
    char delim_ = ',';
};

}  // namespace knp::framework::input
