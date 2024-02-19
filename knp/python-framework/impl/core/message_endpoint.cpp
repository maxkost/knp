/**
 * @file message_endpoint.cpp
 * @brief Message endpoint Python bindings.
 * @author Artiom N.
 * @date 01.02.2024
 */
#include "common.h"


#if defined(_KNP_IN_CORE)
/*
py::class_<core::MessageEndpoint>("MessageEndpoint", "The MessageEndpoint class is a definition of message endpoints.")
    .def("get_receiver_uid", &core::MessageEndpoint::get_receiver_uid, "Get receiver UID from a subscription variant.")
    .def(
        "get_subscription_key", &core::MessageEndpoint::get_subscription_key,
        "Get subscription key from a subscription variant.")
    .def(py::init<core::MessageEndpoint>(), "Move constructor for message endpoints.")
    //        .def(
    //            "operator=", (MessageEndpoint(MessageEndpoint::*)(knp::MessageEndpoint)) &
    //            MessageEndpoint::operator=, "Default copy operator.")
    //        .def("operator=", &MessageEndpoint::operator=, "Default copy operator.")
    //        .def(
    //            "operator=", (MessageEndpoint(MessageEndpoint::*)(knp::MessageEndpoint)) &
    //            MessageEndpoint::operator=, "Avoid copy assignment of an endpoint.")
    .def(
        "subscribe", &core::MessageEndpoint::subscribe,
        "Add a subscription to messages of the specified type from senders with given UIDs.")
    .def("unsubscribe", &core::MessageEndpoint::unsubscribe, "Unsubscribe from messages of a specified type.")
    .def(
        "remove_receiver", &core::MessageEndpoint::remove_receiver,
        "Remove all subscriptions for a receiver with given UID.")
    .def("send_message", &core::MessageEndpoint::send_message, "Send a message to the message bus.")
    .def(
        "unload_messages", &core::MessageEndpoint::unload_messages,
        "Read messages of the specified type received via subscription.")
    .def(
        "receive_all_messages", &core::MessageEndpoint::receive_all_messages,
        "Receive all messages that were sent to the endpoint.");
*/
#endif
