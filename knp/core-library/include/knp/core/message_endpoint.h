/**
 * @file message_bus.h
 * @brief Message bus interface.
 * @author Artiom N.
 * @date 23.01.2023
 */

#pragma once

#include <knp/core/messaging.h>
#include <knp/core/subscription.h>
#include <knp/core/uid.h>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <boost/mp11.hpp>
#include <boost/multi_index_container.hpp>


namespace knp::core
{
namespace mi = boost::multi_index;

/**
 * @brief The MessageEndpoint class is a definition of message endpoints.
 * @details You can use message endpoints to receive or send messages.
 */
class MessageEndpoint
{
public:
    using SupportedMessages = messaging::AllMessages;
    using AllSubscriptions = boost::mp11::mp_transform<Subscription, SupportedMessages>;

    using SubscriptionVariant = boost::mp11::mp_rename<AllSubscriptions, std::variant>;
    using MessageVariant = boost::mp11::mp_rename<SupportedMessages, std::variant>;

public:
    UID get_receiver(const SubscriptionVariant &subscription);
    std::pair<UID, size_t> get_subscription_key(const SubscriptionVariant &subscription);

    template <typename Variant, typename Type>
    static constexpr size_t get_type_index = boost::mp11::mp_find<Variant, Type>::value;

public:
    MessageEndpoint(MessageEndpoint &&endpoint);
    MessageEndpoint &operator=(MessageEndpoint &&endpoint) = default;
    virtual ~MessageEndpoint();

public:
    struct by_type
    {
    };
    struct by_receiver
    {
    };
    struct by_type_and_uid
    {
    };

public:
    /**
     * @brief Add subscription to parameters or update an existing one
     * @tparam MessageType message type the receiver subscribes to
     * @param receiver receiver UID
     * @param senders a vector of sender UIDs
     * @return the number of senders added
     */
    template <typename MessageType>
    size_t subscribe(const UID &receiver, const std::vector<UID> &senders)
    {
        //        constexpr size_t index = get_type_index<SubscriptionVariant, Subscription<MessageType>>;

        //        auto iter = subscriptions_.get<by_type_and_uid>().find(std::make_pair(receiver, index));
        //        if (iter != subscriptions_.get<by_type_and_uid>().end())
        //        {
        //            return std::get<index>(*iter).add_senders(senders);
        //        }
        //        else
        //        {
        //            subscriptions_.emplace(Subscription<MessageType>(receiver, senders));
        //            return senders.size();
        //        }
        return 0;
    }

    /**
     * @brief Unsubscribe from messages of a certain type
     * @param receiver receiver UID.
     */
    template <typename MessageType>
    void unsubscribe(const UID &receiver)
    {
        //        constexpr size_t index = get_type_index<MessageVariant, MessageType>();
        //        auto &sub_list = subscriptions_.get<by_type_and_uid>();
        //        sub_list.erase(sub_list.find(std::make_pair(receiver, index)));
    }

    /**
     * @brief Remove all subscriptions with given reciever id
     * @param receiver_uid UID of the receiver
     */
    void remove_receiver(const UID &receiver_uid);

    /**
     * @brief Send a message to the message bus.
     * @param message message to send.
     */
    void send_message(const MessageVariant &message);

    /**
     * @brief Receive a message from the message bus.
     * @return true if nonempty message was received, false otherwise.
     */
    template <class MessageType>
    bool receive_message();

    //    typedef mi::multi_index_container<
    //        MessageEndpoint::SubscriptionVariant
    //        mi::indexed_by<
    //            mi::ordered_non_unique<
    //                mi::tag<by_type>, BOOST_MULTI_INDEX_CONST_MEM_FUN(SubscriptionVariant, size_t, index)>,
    //            mi::ordered_non_unique<
    //                mi::tag<by_receiver>, mi::global_fun<const SubscriptionVariant &, UID, get_receiver>>,
    //            mi::ordered_unique<
    //                mi::tag<by_type_and_uid>,
    //                mi::composite_key<
    //                    mi::global_fun<const SubscriptionVariant &, UID, get_receiver>,
    //                    mi::const_mem_fun<SubscriptionVariant, size_t, &SubscriptionVariant::index>>>
    //            >
    //        >
    //        SubscriptionContainer;

protected:
    explicit MessageEndpoint(void *context, const std::string &sub_addr, const std::string &pub_addr);

private:
    MessageEndpoint() = delete;

private:
    /// Message endpoint implementation.
    class MessageEndpointImpl;

    //    SubscriptionContainer subscriptions_;
    std::unique_ptr<MessageEndpointImpl> impl_;
};

}  // namespace knp::core
