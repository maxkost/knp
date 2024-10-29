/**
 * @file input_channel.cpp
 * @brief Python bindings for InputChannel.
 * @kaspersky_developer Vartenkov A.
 * @date 30.05.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */


#include "common.h"

#ifdef KNP_IN_BASE_FW

py::class_<knp::framework::io::input::InputChannel, boost::noncopyable>(
    "InputChannel", "The InputChannel class is a definition of an input channel.", py::no_init)
    .def("get_uid", &get_entity_uid<knp::framework::io::input::InputChannel>, "Get backend UID.")
    .def(
        "send", &knp::framework::io::input::InputChannel::send,
        "Read data from input stream, form a spike message and send it to an endpoint.")
    .def("__init__", py::make_constructor(&construct_input_channel), "Initialize the input channel attributes.");


#endif  // KNP_IN_BASE_FW
