/**
 * @file spike_message.cpp
 * @brief Spike message I/O operators implementation.
 * @author Vartenkov A.
 * @date 23.03.2023
 */

#include <spdlog/spdlog.h>

#include "spike_message_impl.h"


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


::flatbuffers::uoffset_t pack_internal(::flatbuffers::FlatBufferBuilder &builder, const SpikeMessage &msg)
{
    SPDLOG_TRACE("Packing spike message...");
    marshal::MessageHeader header{marshal::UID{msg.header_.sender_uid_.tag.data}, msg.header_.send_time_};

    return marshal::CreateSpikeMessageDirect(builder, &header, &msg.neuron_indexes_).o;
}


std::vector<uint8_t> pack(const SpikeMessage &msg)
{
    // TODO: don't create instance every time.
    ::flatbuffers::FlatBufferBuilder builder;
    auto s_msg = std::move(pack_internal(builder, msg));
    marshal::FinishSpikeMessageBuffer(builder, ::flatbuffers::Offset<marshal::SpikeMessage>(s_msg));
    return std::vector<uint8_t>(builder.GetBufferPointer(), builder.GetBufferPointer() + builder.GetSize());
}


SpikeMessage unpack(const marshal::SpikeMessage *s_msg)
{
    SPDLOG_TRACE("Unpacking spike message FlatBuffers class");
    const marshal::MessageHeader *const s_msg_header{s_msg->header()};

    UID u1{false};
    std::copy(s_msg_header->sender_uid().data()->begin(), s_msg_header->sender_uid().data()->end(), u1.tag.data);

    return SpikeMessage{
        {u1, s_msg_header->send_time()}, {s_msg->neuron_indexes()->begin(), s_msg->neuron_indexes()->end()}};
}


/*
SpikeMessage unpack(const void *buffer)
{
    SPDLOG_TRACE("Unpacking spike message buffer");
    const marshal::SpikeMessage *const s_msg{marshal::GetSpikeMessage(buffer)};
    return unpack(s_msg);
}


SpikeMessage unpack(std::vector<uint8_t> &buffer)
{
    SPDLOG_TRACE("Unpacking spike message vector buffer");
    return unpack(buffer.data());
}
*/

}  // namespace knp::core::messaging
