/**
 * @file spike_message.cpp
 * @brief Spike message I/O operators implementation.
 * @author Vartenkov A.
 * @date 23.03.2023
 */

#include <knp/core/messaging/spike_message.h>

#include <knp_gen_headers/spike_message_generated.h>


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


std::vector<uint8_t> pack(const SpikeMessage &msg)
{
    // TODO: doesn't create instance every time.
    ::flatbuffers::FlatBufferBuilder builder;
    marshal::MessageHeader header{marshal::UID{msg.header_.sender_uid_.tag.data}, msg.header_.send_time_};

    auto s_msg = std::move(marshal::CreateSpikeMessageDirect(builder, &header, &msg.neuron_indexes_));
    marshal::FinishSpikeMessageBuffer(builder, s_msg);
    return std::vector<uint8_t>(builder.GetBufferPointer(), builder.GetBufferPointer() + builder.GetSize());
}


template <>
SpikeMessage unpack<SpikeMessage>(std::vector<uint8_t> &buffer)
{
    const marshal::SpikeMessage *const s_msg{marshal::GetSpikeMessage(buffer.data())};
    const marshal::MessageHeader *const s_msg_header{s_msg->header()};

    UID u1{false};
    std::copy(s_msg_header->sender_uid().data()->begin(), s_msg_header->sender_uid().data()->end(), u1.tag.data);

    return SpikeMessage{
        {u1, s_msg_header->send_time()}, {s_msg->neuron_indexes()->begin(), s_msg->neuron_indexes()->end()}};
}

}  // namespace knp::core::messaging
