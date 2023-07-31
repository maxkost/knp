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
#include <sstream>
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
     * @param delim character that delimits spiked neuron indexes.
     */
    explicit IndexConverter(char delim = ',') : delim_(delim) {}

    /**
     * @brief Call a function that converts data from the input stream into spike messages with spiked neuron indexes.
     * @param stream input stream.
     * @return vector of spiked neuron indexes.
     */
    core::messaging::SpikeData operator()(std::istream &stream) const;

private:
    /**
     * @brief Delimiter character.
     */
    const char delim_ = ',';
};

}  // namespace knp::framework::input
