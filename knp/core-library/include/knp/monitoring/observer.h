/**
 * @file observer.h
 * @brief an observer class that unloads messages and stores them.
 * @author Vartenkov A.
 * @date 23.08.2023
 */
#pragma once
#include <knp/core/message_endpoint.h>
#include <knp/core/messaging/messaging.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


namespace knp::monitoring
{


/**
 * @brief simple functor that writes spikes from entities to file in a given order.
 */
class OrderedWriter
{
public:
    /**
     * @brief Constructor.
     * @param path output file path.
     * @param order order of entities.
     * @param separator string that separates spike indexes.
     */
    explicit OrderedWriter(const std::filesystem::path &path, std::vector<core::UID> order, std::string separator = " ")
        : order_(std::move(order)), separator_(std::move(separator))
    {
        file_.open(path);
        if (file_.bad()) throw std::runtime_error(std::string{"Couldn't open file: "} + path.string());
    }

    void operator()(const std::vector<core::messaging::SpikeMessage> &messages)
    {
        // TODO: Inefficient, but should not be a problem
        for (size_t i = 0; i < order_.size(); ++i)
        {
            if (messages[i].header_.sender_uid_ == order_[i])
            {
                for (auto &value : messages[i].neuron_indexes_) file_ << value << separator_;
                file_ << std::endl;
            }
        }
    }

private:
    std::vector<core::UID> order_;
    std::string separator_;
    std::ofstream file_;
};


template <class Message>
class MessageObserver
{
public:
    MessageObserver(core::MessageEndpoint &&endpoint, core::UID uid)
        : endpoint_(std::move(endpoint)), uid_(std::move(uid))
    {
    }

    void subscribe(const std::vector<core::UID> &entities) { endpoint_.subscribe<Message>(uid_, entities); }

    void update()
    {
        endpoint_.receive_all_messages();
        auto messages_raw = endpoint_.unload_messages<Message>(uid_);
        process_messages_(messages_raw);
    }

private:
    std::function<void(std::vector<Message>)> process_messages_;
    core::MessageEndpoint endpoint_;
    core::UID uid_;
};
}  // namespace knp::monitoring
