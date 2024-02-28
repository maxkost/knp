/**
 * @file message_endpoint.cpp
 * @brief Message endpoint Python bindings.
 * @author Artiom N.
 * @date 01.02.2024
 */

#include "message_endpoint.h"

#include <memory>
#include <vector>

#include "common.h"


#if defined(_KNP_IN_CORE)

namespace mt = core::messaging;

// Stringize SpikeMessage -> "SpikeMessages".
#    define INSTANCE_PY_MESSAGE_CONVERTERS(n, template_for_instance, message_type)       \
        py::implicitly_convertible<mt::message_type, core::messaging::MessageVariant>(); \
                                                                                         \
        py::class_<std::vector<mt::message_type>>(                                       \
            BOOST_PP_STRINGIZE(BOOST_PP_CAT(message_type, s)))                           \
                .def(py::vector_indexing_suite<std::vector<mt::message_type>>());


#    define INSTANCE_PY_MESSAGE_ENDPOINT_SUBSCRIBE_METHOD_IMPL(n, template_for_instance, message_type) \
        if (BOOST_PP_STRINGIZE(message_type) == class_obj_name)                                        \
        {                                                                                              \
            SPDLOG_TRACE("Subscribing to: {}", class_obj_name);                                        \
            self.subscribe<mt::message_type>(receiver, py_iterable_to_vector<core::UID>(senders));     \
            return;                                                                                    \
        }

#    define INSTANCE_PY_MESSAGE_ENDPOINT_UNSUBSCRIBE_METHOD_IMPL(n, template_for_instance, message_type) \
        if (BOOST_PP_STRINGIZE(message_type) == class_obj_name) return self.unsubscribe<mt::message_type>(receiver);


#    define INSTANCE_PY_MESSAGE_ENDPOINT_UNLOAD_MESSAGES_METHOD_IMPL(n, template_for_instance, message_type) \
        if (BOOST_PP_STRINGIZE(message_type) == class_obj_name)                                              \
        {                                                                                                    \
            auto msgs = self.unload_messages<mt::message_type>(receiver);                                    \
            SPDLOG_TRACE("Unloading messages of the type {} [count = {}]", class_obj_name, msgs.size());     \
            return py::object(msgs);                                                                         \
        }


py::class_<core::MessageEndpoint, std::shared_ptr<core::MessageEndpoint>, boost::noncopyable>(
    "MessageEndpoint", "The MessageEndpoint class is a definition of message endpoints.", py::no_init)
    // cppcheck-suppress unknownMacro
    .add_property(
        "receiver_uid", &core::MessageEndpoint::get_receiver_uid, "Get receiver UID from a subscription variant.")
    // cppcheck-suppress unknownMacro
    .add_property(
        "subscription_key", &core::MessageEndpoint::get_subscription_key,
        "Get subscription key from a subscription variant.")
    .def(
        "subscribe",
        make_handler(
            [](core::MessageEndpoint &self, const py::object &msg_class, const core::UID &receiver,
               const py::list &senders)  //-> py::object
            {
                const auto class_obj_name = get_py_class_name(msg_class);

                SPDLOG_TRACE("Message class name: {}", class_obj_name);

                // cppcheck-suppress unknownMacro
                BOOST_PP_SEQ_FOR_EACH(
                    INSTANCE_PY_MESSAGE_ENDPOINT_SUBSCRIBE_METHOD_IMPL, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_MESSAGES))

                PyErr_SetString(PyExc_TypeError, "Passed object is not message class!");
                py::throw_error_already_set();

                throw std::runtime_error("Incorrect class!");
            }),
        "Add a subscription to messages of the specified type from senders with given UIDs.")
    .def(
        "unsubscribe",
        make_handler(
            [](core::MessageEndpoint &self, const py::object &msg_class, const core::UID &receiver) -> bool
            {
                const auto class_obj_name = get_py_class_name(msg_class);
                SPDLOG_TRACE("Message class name: {}", class_obj_name);

                // cppcheck-suppress unknownMacro
                BOOST_PP_SEQ_FOR_EACH(
                    INSTANCE_PY_MESSAGE_ENDPOINT_UNSUBSCRIBE_METHOD_IMPL, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_MESSAGES))

                return false;
            }),
        "Unsubscribe from messages of a specified type.")
    .def(
        "unload_messages",
        make_handler(
            [](core::MessageEndpoint &self, const py::object &msg_class, const core::UID &receiver) -> py::object
            {
                const auto class_obj_name = get_py_class_name(msg_class);

                SPDLOG_TRACE("Message class name: {}", class_obj_name);

                // cppcheck-suppress unknownMacro
                BOOST_PP_SEQ_FOR_EACH(
                    INSTANCE_PY_MESSAGE_ENDPOINT_UNLOAD_MESSAGES_METHOD_IMPL, "",
                    BOOST_PP_VARIADIC_TO_SEQ(ALL_MESSAGES))

                PyErr_SetString(PyExc_TypeError, "Passed object is not message class!");
                py::throw_error_already_set();

                throw std::runtime_error("Incorrect class!");
            }),
        "Read messages of the specified type received via subscription.")
    .def(
        "remove_receiver", &core::MessageEndpoint::remove_receiver,
        "Remove all subscriptions for a receiver with given UID.")
    .def("send_message", &core::MessageEndpoint::send_message, "Send a message to the message bus.")
    .def(
        "receive_all_messages",
        make_handler([](core::MessageEndpoint &self) -> size_t { return self.receive_all_messages(); }),
        // receive_all_messages_overloads(py::args("sleep_duration")),
        "Receive all messages that were sent to the endpoint.");


// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_PY_MESSAGE_CONVERTERS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_MESSAGES))

#endif
