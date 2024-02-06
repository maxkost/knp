#if defined(__KNP_IN_CORE)
/*
py::class_<core::Subscription>(
    "Subscription", "The Subscription class is used for message exchange between the network entities.")
    .def(py::init<core::UID, std::vector<core::UID>>(), "Subscription constructor.")
    .def("remove_sender", &core::Subscription::remove_sender, "Unsubscribe from a sender.")
    .def("add_sender", &core::Subscription::add_sender, "Add a sender with the given UID to the subscription.")
    .def("add_senders", &core::Subscription::add_senders, "Add several senders to the subscription.")
    .def("has_sender", &core::Subscription::has_sender, "Check if a sender with the given UID exists.")
    .def(
        "add_message", (void(core::Subscription::*)(core::MessageType)) & core::Subscription::add_message,
        "Add a message to the subscription.")
    .def("add_message", &core::Subscription::add_message, "Add a message to the subscription.")
    .def(
        "add_message", (void(core::Subscription::*)(MessageType)) & core::Subscription::add_message,
        "Add a message to the subscription.")
    .def("add_message", &core::Subscription::add_message, "Add a message to the subscription.")
    .def(
        "get_messages", (core::MessageContainerType(core::Subscription::*)()) & core::Subscription::get_messages, "Get
all messages.") .def( "get_messages", (core::MessageContainerType(core::Subscription::*)()) &
core::Subscription::get_messages, "Get all messages.");
*/

#endif
