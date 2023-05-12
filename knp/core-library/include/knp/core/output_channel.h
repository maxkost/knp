/**
 * @brief Output channel header
 * @author Vartenkov Andrey
 * @date 11.05.2023
 */
#pragma once

#include <knp/core/message_endpoint.h>
#include <knp/core/messaging/messaging.h>
#include <knp/core/output_converter.h>

#include <utility>


/**
 * @brief Network output processing namespace.
 */
namespace knp::core::output
{
class OutputChannelBase
{
public:
    OutputChannelBase(MessageEndpoint &endpoint, UID sender_uid, size_t output_size)
        : endpoint_(endpoint), uid_{true}, output_size_(output_size)
    {
        endpoint_.subscribe<messaging::SpikeMessage>(uid_, {sender_uid});
    }

    /**
     * @brief Sets cutoff step. All messages older than this step will be discarded.
     * @param step new cutoff step.
     */
    void set_cutoff_step(messaging::TimeType step) { oldest_step_ = step; }

    /**
     * @brief Get channel UID.
     * @return channel UID.
     */
    UID get_uid() { return uid_; }

    /**
     * @brief Set channel UID.
     * @param uid new channel UID.
     */
    void set_uid(const UID &uid) { uid_ = uid; }

protected:
    /**
     * @brief All messages older than this step will be discarded. Might as well do it public.
     */
    messaging::TimeType oldest_step_ = 0;

    /**
     * @brief Endpoint reference for message processing.
     */
    MessageEndpoint &endpoint_;

    /**
     * @brief Channel UID.
     */
    UID uid_;

    /**
     * @brief Expected size of the output projection.
     */
    size_t output_size_;
};


/**
 * @brief Output channel class that uses stream interface.
 * @tparam ResultType type of return values
 */
template <class ResultType>
class OutputChannel : public OutputChannelBase
{
public:
    /**
     * @brief Output channel constructor.
     * @param endpoint endpoint for message exchange.
     * @param converter data converter.
     * @param sender_uid UID of the sender population.
     * @param output_size expected population size.
     */
    OutputChannel(MessageEndpoint &endpoint, OutputConverter<ResultType> converter, UID sender_uid, size_t output_size)
        : OutputChannelBase(endpoint, sender_uid, output_size), converter_(std::move(converter))
    {
    }

    /**
     * @brief Set new converter function to channel.
     * @param converter new converter function.
     */
    void set_converter(const OutputConverter<ResultType> &converter) { converter_ = converter; }

    /**
     * @brief Read all accumulated messages from subscription, then convert them to data.
     * @param result
     * @return
     */
    OutputChannelBase &operator>>(ResultType &result)
    {
        auto messages = endpoint_.unload_messages<messaging::SpikeMessage>(uid_);
        result = converter_(messages, output_size_, oldest_step_);
        return *this;
    }

private:
    /**
     * @brief Data converter function.
     */
    OutputConverter<ResultType> converter_;
};

// TODO: Add instantiation for most frequently used result types: int, float, vector<uint>, vector<bool>, mb set<uint>.

}  // namespace knp::core::output
