/**
 * @file message_endpoint.cpp
 * @brief Message endpoint Python bindings.
 * @author Artiom N.
 * @date 01.02.2024
 */

#include "message_endpoint.h"

#include <memory>

#include "common.h"

// endpoint.subscribe<knp::core::messaging::SpikeMessage>(out_channel_uid, {population.get_uid()})

#if defined(_KNP_IN_CORE)

py::implicitly_convertible<core::messaging::SpikeMessage, core::messaging::MessageVariant>();

py::class_<core::MessageEndpoint, std::shared_ptr<core::MessageEndpoint>, boost::noncopyable>(
    "MessageEndpoint", "The MessageEndpoint class is a definition of message endpoints.", py::no_init)
    .add_property(
        "receiver_uid", &core::MessageEndpoint::get_receiver_uid, "Get receiver UID from a subscription variant.")
    .add_property(
        "subscription_key", &core::MessageEndpoint::get_subscription_key,
        "Get subscription key from a subscription variant.")
    .def(
        "subscribe",
        make_handler(
            [](core::MessageEndpoint &self, const py::object &msg_class, const core::UID &receiver,
               const py::list &senders) -> core::Subscription<core::messaging::SpikeMessage> &
            {
                auto class_obj_name = get_py_class_name(msg_class);

                SPDLOG_TRACE("Message class name: {}", class_obj_name);

                if (class_obj_name != "SpikeMessage")
                {
                    PyErr_SetString(PyExc_TypeError, "Passed object is not message class!");
                    py::throw_error_already_set();
                    return;
                }

                return self.subscribe<knp::core::messaging::SpikeMessage>(
                    receiver, py_iterable_to_vector<core::UID>(senders));
            }),
        "Add a subscription to messages of the specified type from senders with given UIDs.")
    .def(
        "unsubscribe",
        make_handler(
            [](core::MessageEndpoint &self, const core::messaging::SpikeMessage &, const core::UID &receiver) -> bool
            { return self.unsubscribe<knp::core::messaging::SpikeMessage>(receiver); }),
        "Unsubscribe from messages of a specified type.")
    //    .def(
    //        "unload_messages",
    //
    //            template <class MessageType>
    //    std::vector<MessageType> unload_messages(const knp::core::UID &receiver_uid)
    //        ,
    //        "Read messages of the specified type received via subscription.")
    .def(
        "remove_receiver", &core::MessageEndpoint::remove_receiver,
        "Remove all subscriptions for a receiver with given UID.")
    .def("send_message", &core::MessageEndpoint::send_message, "Send a message to the message bus.")
    .def(
        "receive_all_messages", &core::MessageEndpoint::receive_all_messages,  // receive_all_messages_overloads(),
        "Receive all messages that were sent to the endpoint.");

// #define INSTANCE_PY_MESSAGE_ENDPOINT_METHODS(n, template_for_instance, neuron_type)

// cppcheck-suppress unknownMacro
// BOOST_PP_SEQ_FOR_EACH(INSTANCE_PY_MESSAGE_ENDPOINT_METHODS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_MESSAGES))

#endif
