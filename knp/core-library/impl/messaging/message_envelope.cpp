/**
 * @file message_envelope.cpp
 * @brief Message envelope implementation.
 * @author Artiom N.
 * @date 13.04.2023
 */

#include <knp/core/messaging/message_envelope.h>

#include <knp_gen_headers/message_envelope_generated.h>
#include <spdlog/spdlog.h>

#include "spike_message_impl.h"
#include "synaptic_impact_message_impl.h"


namespace knp::core::messaging
{

std::vector<uint8_t> pack_to_envelope(boost::mp11::mp_rename<AllMessages, std::variant> message)
{
    ::flatbuffers::FlatBufferBuilder builder;
    ::flatbuffers::Offset<marshal::MessageEnvelope> s_msg;

    std::visit(
        [&builder, &s_msg](const auto &msg)
        {
            // Zero index is NONE.
            const auto message_type = +boost::mp11::mp_find<AllMessages, decltype(msg)>();
            SPDLOG_TRACE("Creating envelope for the message type {}", message_type);
            s_msg = marshal::CreateMessageEnvelope(
                builder, static_cast<marshal::Message>(message_type), pack_internal(builder, msg));
            marshal::FinishMessageEnvelopeBuffer(builder, s_msg);
        },
        message);

    return std::vector<uint8_t>(builder.GetBufferPointer(), builder.GetBufferPointer() + builder.GetSize());
}


boost::mp11::mp_rename<AllMessages, std::variant> extract_from_envelope(const void *buffer)
{
    auto msg_ev = marshal::GetMessageEnvelope(buffer);

    switch (msg_ev->message_type())
    {
        case marshal::Message_SpikeMessage:
            SPDLOG_TRACE("Unpacking spike message from the envelope");
            return unpack<SpikeMessage>(msg_ev->message());
        case marshal::Message_SynapticImpactMessage:
            SPDLOG_TRACE("Unpacking synaptic impact message from the envelope");
            return unpack<SynapticImpactMessage>(msg_ev->message());
        default:
            SPDLOG_ERROR("Unknown message type {}!", static_cast<int>(msg_ev->message_type()));
            throw std::logic_error("Unknown message type!");
    }
}

boost::mp11::mp_rename<AllMessages, std::variant> extract_from_envelope(std::vector<uint8_t> &buffer)
{
    return extract_from_envelope(buffer.data());
}

}  // namespace knp::core::messaging
