/**
 * @file spike_message.cpp
 * @brief Spike message I/O operators implementation.
 * @author Vartenkov A.
 * @date 23.03.2023
 */

#include <knp/core/messaging/spike_message.h>

namespace knp::core::messaging
{

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
