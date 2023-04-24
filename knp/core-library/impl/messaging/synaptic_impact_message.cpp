/**
 * @file synaptic_impact_message.cpp
 * @brief Synaptic impact message I/O operators implementation.
 * @author Vartenkov A.
 * @date 23.03.2023
 */

#include <spdlog/spdlog.h>

#include <algorithm>

#include "synaptic_impact_message_impl.h"


namespace knp::core::messaging
{

bool SynapticImpact::operator==(const SynapticImpact &other) const
{
    return connection_index_ == other.connection_index_ &&
           presynaptic_neuron_index_ == other.presynaptic_neuron_index_ &&
           postsynaptic_neuron_index_ == other.postsynaptic_neuron_index_ && impact_value_ == other.impact_value_;
}


std::istream &operator>>(std::istream &stream, SynapticImpact &impact)
{
    int type;
    stream >> impact.connection_index_ >> impact.impact_value_ >> type >> impact.presynaptic_neuron_index_ >>
        impact.postsynaptic_neuron_index_;
    impact.synapse_type_ = static_cast<knp::synapse_traits::OutputType>(type);
    return stream;
}


std::ostream &operator<<(std::ostream &stream, const SynapticImpact &impact)
{
    stream << impact.connection_index_ << " " << impact.impact_value_ << " " << static_cast<int>(impact.synapse_type_)
           << " " << impact.presynaptic_neuron_index_ << " " << impact.postsynaptic_neuron_index_;
    return stream;
}


std::ostream &operator<<(std::ostream &stream, const SynapticImpactMessage &msg)
{
    stream << msg.header_ << " " << msg.postsynaptic_population_uid_ << " " << msg.presynaptic_population_uid_ << " "
           << msg.impacts_.size();
    for (auto v : msg.impacts_) stream << " " << v;
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


::flatbuffers::uoffset_t pack_internal(::flatbuffers::FlatBufferBuilder &builder, const SynapticImpactMessage &msg)
{
    SPDLOG_TRACE("Packing synaptic impact message...");
    marshal::MessageHeader header{marshal::UID{msg.header_.sender_uid_.tag.data}, msg.header_.send_time_};

    std::vector<knp::core::messaging::marshal::SynapticImpact> impacts;
    impacts.reserve(msg.impacts_.size());

    std::transform(
        msg.impacts_.begin(), msg.impacts_.end(), std::back_inserter(impacts),
        [](const auto &e)
        {
            auto type = static_cast<knp::synapse_traits::marshal::OutputType>(e.synapse_type_);
            return knp::core::messaging::marshal::SynapticImpact{
                e.connection_index_, e.impact_value_, type, e.presynaptic_neuron_index_, e.postsynaptic_neuron_index_};
        });

    auto pre_synaptic_uid = marshal::UID{msg.presynaptic_population_uid_.tag.data};
    auto post_synaptic_uid = marshal::UID{msg.postsynaptic_population_uid_.tag.data};

    return marshal::CreateSynapticImpactMessageDirect(builder, &header, &pre_synaptic_uid, &post_synaptic_uid, &impacts)
        .o;
}


std::vector<uint8_t> pack(const SynapticImpactMessage &msg)
{
    // TODO: don't create instance every time.
    ::flatbuffers::FlatBufferBuilder builder;
    auto s_msg = std::move(pack_internal(builder, msg));
    marshal::FinishSynapticImpactMessageBuffer(
        builder, static_cast<::flatbuffers::Offset<marshal::SynapticImpactMessage>>(s_msg));
    return std::vector<uint8_t>(builder.GetBufferPointer(), builder.GetBufferPointer() + builder.GetSize());
}


SynapticImpactMessage unpack(const marshal::SynapticImpactMessage *s_msg)
{
    SPDLOG_TRACE("Unpacking synaptic impact message FlatBuffers class");
    const marshal::MessageHeader *const s_msg_header{s_msg->header()};

    UID sender_uid{false};
    UID presynaptic_uid{false};
    UID postsynaptic_uid{false};

    std::copy(
        s_msg_header->sender_uid().data()->begin(), s_msg_header->sender_uid().data()->end(), sender_uid.tag.data);
    std::copy(
        s_msg->presynaptic_population_uid()->data()->begin(), s_msg->presynaptic_population_uid()->data()->end(),
        presynaptic_uid.tag.data);
    std::copy(
        s_msg->postsynaptic_population_uid()->data()->begin(), s_msg->postsynaptic_population_uid()->data()->end(),
        postsynaptic_uid.tag.data);

    std::vector<SynapticImpact> impacts;
    impacts.reserve(s_msg->impacts()->size());

    std::transform(
        s_msg->impacts()->begin(), s_msg->impacts()->end(), std::back_inserter(impacts),
        [](const auto &e)
        {
            const auto type = static_cast<knp::synapse_traits::OutputType>(e->output_type());
            return SynapticImpact{
                e->connection_index(), e->impact_value(), type, e->presynaptic_neuron_index(),
                e->postsynaptic_neuron_index()};
        });

    return SynapticImpactMessage{
        {sender_uid, s_msg_header->send_time()}, presynaptic_uid, postsynaptic_uid, std::move(impacts)};
}

}  // namespace knp::core::messaging
