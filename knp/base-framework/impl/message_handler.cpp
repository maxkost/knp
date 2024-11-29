//
// Created by an_vartenkov on 22.11.24.
//
#include <knp/framework/message_handler.h>

#include <unordered_set>
#include <utility>

namespace knp::framework::modifier
{

void SpikeMessageHandler::update(size_t step)
{
    endpoint_.receive_all_messages();
    auto incoming_messages = endpoint_.unload_messages<MessageIn>(base_.uid_);
    MessageOut outgoing_message = {{base_.uid_, step}, message_handler_function_(incoming_messages)};
    if (!(outgoing_message.neuron_indexes_.empty()))
    {
        endpoint_.send_message(outgoing_message);
    }
}


knp::core::messaging::SpikeData KWtaRandomHandler::operator()(std::vector<knp::core::messaging::SpikeMessage> &messages)
{
    if (messages.empty()) return {};

    auto &msg = messages[0];
    if (msg.neuron_indexes_.size() < num_winners_) return msg.neuron_indexes_;

    knp::core::messaging::SpikeData out_spikes;
    for (size_t i = 0; i < num_winners_; ++i)
    {
        const size_t index = distribution_(random_engine_) % (msg.neuron_indexes_.size() - i);
        out_spikes.push_back(msg.neuron_indexes_[index]);
        std::swap(msg.neuron_indexes_[index], msg.neuron_indexes_[msg.neuron_indexes_.size() - 1 - i]);
    }
    return out_spikes;
}


knp::core::messaging::SpikeData GroupWtaRandomHandler::operator()(
    const std::vector<knp::core::messaging::SpikeMessage> &messages)
{
    if (messages.empty()) return {};
    if (num_winners_ > group_borders_.size()) return messages[0].neuron_indexes_;

    const auto &spikes = messages[0].neuron_indexes_;
    if (spikes.empty()) return {};

    std::vector<std::vector<size_t>> spikes_per_group(group_borders_.size() + 1);

    // Fill groups in
    for (auto spike : spikes)
    {
        const size_t group_index =
            std::upper_bound(group_borders_.begin(), group_borders_.end(), spike) - group_borders_.begin();
        spikes_per_group[group_index].push_back(spike);
    }

    // Sort groups by number of elements
    std::sort(
        spikes_per_group.begin(), spikes_per_group.end(),
        [](const auto &el1, const auto &el2) { return el1.size() > el2.size(); });

    // Find all groups with the same number of spikes as the K-th one
    const auto &last_group = spikes_per_group[num_winners_ - 1];
    auto group_interval = std::equal_range(
        spikes_per_group.begin(), spikes_per_group.end(), last_group,
        [](const auto &el1, const auto &el2) { return el1.size() > el2.size(); });
    const size_t already_decided = group_interval.first - spikes_per_group.begin() + 1;
    assert(already_decided <= num_winners_);
    // The approach could be more efficient, but I don't think it's necessary.
    std::shuffle(group_interval.first, group_interval.second, random_engine_);
    knp::core::messaging::SpikeData result;
    for (size_t i = 0; i < num_winners_; ++i)
    {
        for (auto spike : spikes_per_group[i]) result.push_back(spike);
    }
    return result;
}


knp::core::messaging::SpikeData SpikeUnionHandler::operator()(
    const std::vector<knp::core::messaging::SpikeMessage> &messages)
{
    std::unordered_set<size_t> spikes;
    for (const auto &msg : messages)
    {
        spikes.insert(msg.neuron_indexes_.begin(), msg.neuron_indexes_.end());
    }
    knp::core::messaging::SpikeData result;
    result.reserve(spikes.size());
    std::copy(spikes.begin(), spikes.end(), std::back_inserter(result));
    return result;
}
}  // namespace knp::framework::modifier
