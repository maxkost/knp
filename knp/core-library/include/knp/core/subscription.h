/**
 * @file subscription.h
 * @brief defines Subscription, a class that determines message exchange between entities in the network
 * @author Vartenkov A.
 * @date 15.03.2023
 */

#pragma once
#include <knp/core/uid.h>

#include <algorithm>
#include <unordered_set>
#include <vector>

#include <boost/container_hash/hash.hpp>

namespace knp::core
{

/**
 * @brief A subscription class for message exchange among network entities
 * @tparam MessageType
 */
template <class MessageType>
class Subscription
{
public:
    Subscription(const UID &reciever, const std::vector<UID> &senders)
        : receiver_(reciever), senders_(senders.begin(), senders.end())
    {
    }

    const auto &get_senders() { return senders_; }
    UID get_receiver() { return receiver_; }

    /**
     * Unsubscribe from a sender. If not subscribed to the sender, do nothing.
     * @param uid sender UID
     */
    size_t remove_sender(const UID &uid) { return senders_.erase(uid); }

    bool has_sender() {}

private:
    std::unordered_set<UID, UID_hash> senders_;
    UID receiver_;
    std::vector<MessageType> messages_;
};


}  // namespace knp::core
