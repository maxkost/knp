/**
 * @brief Input index converter header
 * @author Vartenkov Andrey
 * @date 25.04.2023
 */
#pragma once

#include <knp/core/message_endpoint.h>
#include <knp/core/messaging/spike_message.h>
#include <knp/core/uid.h>

#include <spdlog/spdlog.h>

#include <functional>
#include <istream>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


/**
 * @brief Input channel namespace.
 */
namespace knp::framework::input
{
/**
 * @brief The IndexConverter class is a definition of a converter that converts lines of integers into spiked neuron
 * indexes.
 */
class IndexConverter
{
public:
    /**
     * @brief Create converter with a delimiter.
     * @param stream stream from which to receive data.
     * @param delim character that delimits spiked neuron indexes.
     */
    explicit IndexConverter(std::unique_ptr<std::istream> &&stream, char delim = ',')
        : stream_(std::move(stream)), delim_(delim)
    {
    }

    /**
     * @brief Call a function that converts data from the input stream into spike messages with spiked neuron indexes.
     * @param step current step. Doesn't used.
     * @return vector of spiked neuron indexes.
     */
    core::messaging::SpikeData operator()(core::messaging::Step step = 0) const;

public:
    /**
     * @brief Get input stream.
     * @return stream.
     */
    [[nodiscard]] std::istream &get_stream() { return *stream_; }

private:
    /**
     * @brief A reference to the stream from which to read data.
     */
    std::unique_ptr<std::istream> stream_;
    /**
     * @brief Delimiter character.
     */
    const char delim_ = ',';
};

}  // namespace knp::framework::input
