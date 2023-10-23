#include <knp/core-library/include/knp/core/message_endpoint.h>

#include <boost/python.hpp>


namespace py = boost::python;


BOOST_PYTHON_MODULE(MessageEndpoint)
{
    py::class_<MessageEndpoint>("MessageEndpoint", "The MessageEndpoint class is a definition of message endpoints.")
        .def("get_receiver_uid", &MessageEndpoint::get_receiver_uid, "Get receiver UID from a subscription variant.")
        .def(
            "get_subscription_key", &MessageEndpoint::get_subscription_key,
            "Get subscription key from a subscription variant.")
        .def(py::init<knp::MessageEndpoint>(), "Move constructor for message endpoints.")
        //        .def(
        //            "operator=", (MessageEndpoint(MessageEndpoint::*)(knp::MessageEndpoint)) &
        //            MessageEndpoint::operator=, "Default copy operator.")
        //        .def("operator=", &MessageEndpoint::operator=, "Default copy operator.")
        //        .def(
        //            "operator=", (MessageEndpoint(MessageEndpoint::*)(knp::MessageEndpoint)) &
        //            MessageEndpoint::operator=, "Avoid copy assignment of an endpoint.")
        .def(
            "subscribe", &MessageEndpoint::subscribe,
            "Add a subscription to messages of the specified type from senders with given UIDs.")
        .def("unsubscribe", &MessageEndpoint::unsubscribe, "Unsubscribe from messages of a specified type.")
        .def(
            "remove_receiver", &MessageEndpoint::remove_receiver,
            "Remove all subscriptions for a receiver with given UID.")
        .def("send_message", &MessageEndpoint::send_message, "Send a message to the message bus.")
        .def(
            "unload_messages", &MessageEndpoint::unload_messages,
            "Read messages of the specified type received via subscription.")
        .def(
            "receive_all_messages", &MessageEndpoint::receive_all_messages,
            "Receive all messages that were sent to the endpoint.");
}
