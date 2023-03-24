/**
 * @file messaging.cpp
 * @brief Message bus interface.
 * @author Vartemkov A.
 * @date 23.03.2023
 */

#include <knp/core/messaging.h>

namespace knp::core::messaging
{

std::istream &operator>>(std::istream &stream, MessageHeader &header)
{
    stream >> header.sender_uid_ >> header.send_time_;
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const MessageHeader &header)
{
    stream << header.sender_uid_ << header.send_time_;
    return stream;
}

std::istream &operator>>(std::istream &stream, SynapticImpact &impact)
{
    stream >> impact.connection_index_ >> impact.impact_value_ >> impact.presynaptic_neuron_index_ >>
        impact.postsynaptic_neuron_index_;
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const SynapticImpact &impact)
{
    stream << impact.connection_index_ << impact.impact_value_ << impact.presynaptic_neuron_index_
           << impact.postsynaptic_neuron_index_;
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const SynapticImpactMessage &msg)
{
    stream << msg.header_ << msg.postsynaptic_population_uid_ << msg.presynaptic_population_uid_ << msg.impacts_.size();
    for (auto v : msg.impacts_) stream << v;
    return stream;
}

std::istream &operator>>(std::istream &stream, SynapticImpactMessage &msg)
{
    size_t n = 0;
    stream >> msg.header_ >> msg.postsynaptic_population_uid_ >> msg.presynaptic_population_uid_ >> n;
    msg.impacts_.resize(n);
    for (size_t i = 0; i < n; ++i)
    {
        stream >> msg.impacts_[i];
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const SpikeMessage &msg)
{
    stream << msg.header_.sender_uid_ << msg.header_.send_time_ << msg.neuron_indexes_.size();
    for (auto n : msg.neuron_indexes_) stream << n;
    return stream;
}

std::istream &operator>>(std::istream &stream, SpikeMessage &msg)
{
    size_t n = 0;
    stream >> msg.header_.sender_uid_ >> msg.header_.send_time_ >> n;
    msg.neuron_indexes_.resize(n);
    for (size_t i = 0; i < n; ++i)
    {
        stream >> msg.neuron_indexes_[i];
    }
    return stream;
}

}  // namespace knp::core::messaging
