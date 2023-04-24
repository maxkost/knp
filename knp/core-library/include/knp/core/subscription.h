/**
 * @file subscription.h
 * @brief defines Subscription, a class that determines message exchange between entities in the network
 * @author Vartenkov A.
 * @date 15.03.2023
 */

#pragma once
#include <knp/core/uid.h>

#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>

#include <boost/functional/hash.hpp>


namespace knp::core
{

/**
 * @brief The Subscription class is used for message exchange between the network entities.
 * @tparam MessageT type of messages that are exchanged via the subscription.
 */
template <class MessageT>
class Subscription
{
public:
    using MessageType = MessageT;
    using MessageContainerType = std::vector<MessageType>;
    using UidSet = std::unordered_set<::boost::uuids::uuid, boost::hash<boost::uuids::uuid>>;
    // Subscription(const Subscription &) = delete;

public:
    /**
     * @brief Subscription constructor
     * @param receiver UID of the receiving object
     * @param senders a list of senders
     */
    Subscription(const UID &receiver, const std::vector<UID> &senders) : receiver_(receiver) { add_senders(senders); }


    /**
     * @brief Get a list of sender UIDs.
     */
    [[nodiscard]] const UidSet &get_senders() const { return senders_; }

    /**
     * @brief Get receiver UID.
     */
    [[nodiscard]] UID get_receiver_uid() const { return receiver_; }

    /**
     * @brief Unsubscribe from a sender. If not subscribed to the sender, do nothing.
     * @param uid sender UID.
     * @return number of senders deleted from subscription, either 1 or 0.
     */
    size_t remove_sender(const UID &uid) { return senders_.erase(static_cast<boost::uuids::uuid>(uid)); }

    /**
     * @brief Add a sender with the given UID to the subscription.
     * @param uid UID of the new sender.
     * @return 1 if the sender was added to the subscription successfully.
     *         0 if the sender with the given UID is already using the subscription.
     */
    size_t add_sender(const UID &uid) { return senders_.insert(static_cast<boost::uuids::uuid>(uid)).second; }

    /**
     * @brief Add a number of senders to the subscription.
     * @param senders a vector of sender UIDs.
     * @return number of new senders added.
     */
    size_t add_senders(const std::vector<UID> &senders)
    {
        size_t size_before = senders_.size();
        std::copy(senders.begin(), senders.end(), std::inserter(senders_, senders_.end()));
        return senders_.size() - size_before;
    }

    /**
     * @brief Check if a sender with the given UID exists.
     * @param uid sender UID.
     * @return true if the sender with the given UID exists.
     *         false if the sender with the given UID doesn't exist.
     */
    [[nodiscard]] bool has_sender(const UID &uid) const
    {
        return senders_.find(static_cast<boost::uuids::uuid>(uid)) != senders_.end();
    }

public:
    /**
     * @brief Add message to the subscription.
     * @param message the message to add.
     */
    void add_message(MessageType &&message) { messages_.push_back(message); }
    void add_message(const MessageType &message) { messages_.push_back(message); }

    /**
     * @brief Get all messages.
     * @return messages.
     */
    MessageContainerType &get_messages() { return messages_; }
    const MessageContainerType &get_messages() const { return messages_; }

    /**
     * @brief Remove all stored messages.
     */
    void clear_messages() { messages_.clear(); }

private:
    /**
     * @brief Receiver UID.
     */
    const UID receiver_;

    /**
     * @brief Set of sender UIDs.
     */
    std::unordered_set<::boost::uuids::uuid, boost::hash<boost::uuids::uuid>> senders_;
    /**
     * @brief Message storage.
     */
    MessageContainerType messages_;
};

}  // namespace knp::core
