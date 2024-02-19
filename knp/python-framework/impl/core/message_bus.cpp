/**
 * @file message_bus.cpp
 * @brief Message bus Python bindings.
 * @author Artiom N.
 * @date 01.02.2024
 */
#include "common.h"


#if defined(_KNP_IN_CORE)

/*
py::class_<core::MessageBus>("MessageBus", "The MessageBus class is a definition of an interface to a message bus.")
    .def(py::init<>(), "Default message bus constructor.")
    .def(py::init<bool>(), "MessageBus with selection of implementation.")
    .def(
        py::init<std::unique_ptr<messaging::impl::MessageBusImpl>>(),
        "Message bus constructor with a specialized implementation.");
*/

#endif
