/**
 * @file message_bus_single_cpu_impl.h
 * @brief Single-threaded CPU message bus implementation header.
 * @author Vartenkov A.
 * @date 18.09.2023
 */
#pragma once

#include <knp/core/message_endpoint.h>

#include <algorithm>
#include <mutex>
#include <utility>
#include <vector>

/**
 * @brief Core library namespace.
 */
namespace knp::core
{

class MessageEndpointCPUImpl : public core::MessageEndpointImpl
{
public:
    MessageEndpointCPUImpl() {}

    void send_message(const knp::core::messaging::MessageVariant &message) override
    {
        std::lock_guard lock(mutex_);
        messages_to_send_.push_back(message);
    }

    ~MessageEndpointCPUImpl() = default;

    /**
     * @brief Reads all the messages queued to be sent, then clears message container.
     * @return a vector of messages to be sent to other endpoints.
     */
    [[nodiscard]] std::vector<knp::core::messaging::MessageVariant> unload_sent_messages()
    {
        std::lock_guard lock(mutex_);
        auto result = std::move(messages_to_send_);
        messages_to_send_.clear();
        return result;
    }

    // Embarrassingly inefficient: all endpoints basically receive all messages by copying them. Should do better.
    void add_received_messages(const std::vector<knp::core::messaging::MessageVariant> &incoming_messages)
    {
        std::lock_guard lock(mutex_);
        if (received_messages_.capacity() < received_messages_.size() + incoming_messages.size())
            received_messages_.reserve(std::max(2 * received_messages_.size(), 2 * incoming_messages.size()));

        received_messages_.insert(received_messages_.end(), incoming_messages.begin(), incoming_messages.end());
    }

    void add_received_message(knp::core::messaging::MessageVariant &&incoming)
    {
        std::lock_guard lock(mutex_);
        received_messages_.emplace_back(incoming);
    }

    void add_received_message(const knp::core::messaging::MessageVariant &incoming)
    {
        std::lock_guard lock(mutex_);
        received_messages_.push_back(incoming);
    }

    std::optional<knp::core::messaging::MessageVariant> receive_message() override
    {
        std::lock_guard lock(mutex_);
        if (received_messages_.empty()) return {};
        auto result = std::move(received_messages_.back());
        received_messages_.pop_back();
        return result;
    }

private:
    std::vector<messaging::MessageVariant> messages_to_send_;
    std::vector<messaging::MessageVariant> received_messages_;
    std::mutex mutex_;
};

}  // namespace knp::core
