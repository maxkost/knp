/**
 * Message functions testing
 */

#include <knp/core/messaging/messaging.h>

#include <tests_common.h>

#include <sstream>


TEST(MessageSuite, SpikeToChannelTest)
{
    const knp::core::UID uid(true);
    const size_t time = 7;
    const knp::core::messaging::SpikeData indices = {1, 2, 3, 4, 5};

    knp::core::messaging::SpikeMessage message_in{{uid, time}, indices};
    knp::core::messaging::SpikeMessage message_out;
    std::stringstream stream;

    stream << message_in;
    stream >> message_out;

    ASSERT_EQ(uid, message_out.header_.sender_uid_);
    ASSERT_EQ(message_out.header_.send_time_, 7);
    ASSERT_EQ(message_out.neuron_indexes_, indices);
}


TEST(MessageSuite, ImpactToChannelTest)
{
    const knp::core::UID uid{true}, pre_uid{true}, post_uid{true};
    const size_t time = 7;
    const knp::synapse_traits::OutputType type = knp::synapse_traits::OutputType::DOPAMINE;
    const std::vector<knp::core::messaging::SynapticImpact> impacts{{1, 2, type, 3, 4}, {5, 6, type, 7, 8}};

    knp::core::messaging::SynapticImpactMessage message_in{{uid, time}, pre_uid, post_uid, impacts};
    knp::core::messaging::SynapticImpactMessage message_out;

    std::stringstream stream;

    stream << message_in;
    stream >> message_out;

    ASSERT_EQ(message_out.header_.sender_uid_, uid);
    ASSERT_EQ(message_out.header_.send_time_, time);
    ASSERT_EQ(message_out.presynaptic_population_uid_, pre_uid);
    ASSERT_EQ(message_out.postsynaptic_population_uid_, post_uid);
    ASSERT_EQ(message_out.impacts_, impacts);
}
