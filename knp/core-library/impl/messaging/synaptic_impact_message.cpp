/**
 * @file synaptic_impact_message.cpp
 * @brief Synaptic impact message I/O operators implementation.
 * @author Vartenkov A.
 * @date 23.03.2023
 */

#include <knp/core/messaging/synaptic_impact_message.h>

namespace knp::core::messaging
{

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

}  // namespace knp::core::messaging
