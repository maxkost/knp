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
 * @brief Input channel namespace.
 */
namespace knp::framework::input
{
/**
 * @brief The SequenceConverter class is a definition of a stream-like converter that converts a list of input values into spike messages.
 * @details For example, `SequenceConverter<float> converter{interpreter_with_threshold<float>(1.0f)}` 
 * constructs a converter that interprets the input data as a spike if it is equal or greater than the threshold value.
 * @tparam value_type type of values received from an input stream.
 */
template <class value_type>
class SequenceConverter
{
public:
    /**
     * @brief Sequence converter constructor.
     * @param interpret function that determines if the unprocessed value is a spike or not.
     * @param data_size size of input projection.
     */
    SequenceConverter(std::function<bool(value_type)> interpret, size_t data_size)
        : interpret_(std::move(interpret)), data_size_(data_size)
    {
    }

    /**
     * @brief Call a function that converts data from the input stream into spike messages with spiked neuron indexes.
     * @param stream input stream.
     * @return vector of spiked neuron indexes.
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
     * @brief Set input data size.
     * @details The `size` value must correspond to the size of an input projection.
     * @param size input data size.
     */
    void set_size(size_t size) { data_size_ = size; }

private:
    /**
     * @brief Interpretation function that returns `true` if an input value is a spike, otherwise `false`. Any "wrong symbol"
     * error processing logic also goes here.
     */
    std::function<bool(value_type)> interpret_;

    /**
     * @brief Input projection size.
     */
    size_t data_size_;
};


/**
 * @brief The IndexConverter class is a definition of a converter that converts lines of integers into spiked neuron indexes.
 */
class IndexConverter
{
public:
    /**
     * @brief Create converter with a delimiter.
     * @param delim character that delimits spiked neuron indexes.
     */
    explicit IndexConverter(char delim = ',') : delim_(delim) {}

    /**
     * @brief Call a function that converts data from the input stream into spike messages with spiked neuron indexes.
     * @param stream input stream.
     * @return vector of spiked neuron indexes.
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
     * @brief Delimiter character.
     */
    const char delim_ = ',';
};

}  // namespace knp::framework::input
