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
 * @brief Construct a function to turn values to spikes.
 * @tparam value_type input value type, should have a defined operator "less than".
 * @param threshold threshold value, spikes when greater or equal than threshold.
 * @return a simple function to determine if a value causes a spike or not.
 */
template <class value_type>
auto interpret_with_threshold(value_type threshold)
{
    // We're using !(a < b) instead of (a >= b) because of the values which only have operator "less than".
    return [threshold](value_type v) -> bool { return !(v < threshold); };
}

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
     * @param data_size size of input projection
     */
    SequenceConverter(std::function<bool(value_type)> interpret, size_t data_size)
        : interpret_(std::move(interpret)), data_size_(data_size)
    {
    }

    /**
     * @brief Output stream operator.
     * @param stream input stream.
     * @param data_size expected size of the input projection.
     * @return list of neuron indexes.
     */
    std::vector<uint32_t> operator()(std::istream &stream)
    {
        SPDLOG_TRACE("Getting message from a stream using sequence converter.");

        std::vector<uint32_t> message_data;
        for (size_t i = 0; i < data_size_; ++i)
        {
            value_type value;
            stream >> value;
            if (interpret_(value))
            {
                message_data.push_back(i);
            }
        }
        SPDLOG_TRACE(
            std::string("Finished loading a message, it contains ") + std::to_string(message_data.size()) + " spikes.");
        return message_data;
    }

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

}  // namespace knp::framework::input
