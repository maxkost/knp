#include <knp/core-library/include/knp/core/subscription.h>

#include <boost/python.hpp>


namespace py = boost::python;


BOOST_PYTHON_MODULE(Subscription)
{
    py::class_<Subscription>(
        "Subscription", "The Subscription class is used for message exchange between the network entities.")
        .def(py::init<UID, std::vector<UID>>(), "Subscription constructor.")
        .def("remove_sender", &Subscription::remove_sender, "Unsubscribe from a sender.")
        .def("add_sender", &Subscription::add_sender, "Add a sender with the given UID to the subscription.")
        .def("add_senders", &Subscription::add_senders, "Add several senders to the subscription.")
        .def("has_sender", &Subscription::has_sender, "Check if a sender with the given UID exists.")
        .def(
            "add_message", (void(Subscription::*)(MessageType)) & Subscription::add_message,
            "Add a message to the subscription.")
        .def("add_message", &Subscription::add_message, "Add a message to the subscription.")
        .def(
            "add_message", (void(Subscription::*)(MessageType)) & Subscription::add_message,
            "Add a message to the subscription.")
        .def("add_message", &Subscription::add_message, "Add a message to the subscription.")
        .def(
            "get_messages", (MessageContainerType(Subscription::*)()) & Subscription::get_messages, "Get all messages.")
        .def(
            "get_messages", (MessageContainerType(Subscription::*)()) & Subscription::get_messages,
            "Get all messages.");
}
