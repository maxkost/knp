/**
 * @file message_bus.cpp
 * @brief Message bus Python bindings.
 * @author Artiom N.
 * @date 01.02.2024
 */
#include "common.h"


#if defined(_KNP_IN_CORE)

py::class_<core::MessageBus, boost::noncopyable>(
    "MessageBus", "The MessageBus class is a definition of an interface to a message bus.", py::no_init)
    .def(
        "create_endpoint",
        make_handler([](core::MessageBus &self)
                     { return std::make_shared<core::MessageEndpoint>(std::move(self.create_endpoint())); }),
        "Create a new endpoint that sends and receives messages through the message bus")
    .def("step", &core::MessageBus::step, "Route some messages")
    .def("route_messages", &core::MessageBus::route_messages, "Route messages");

#endif
