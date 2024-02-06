#include "common.h"


#if defined(__KNP_IN_CORE)

/*
py::class_<core::MessageBus>("MessageBus", "The MessageBus class is a definition of an interface to a message bus.")
    .def(py::init<>(), "Default message bus constructor.")
    .def(py::init<bool>(), "MessageBus with selection of implementation.")
    .def(
        py::init<std::unique_ptr<messaging::impl::MessageBusImpl>>(),
        "Message bus constructor with a specialized implementation.");
*/

#endif
