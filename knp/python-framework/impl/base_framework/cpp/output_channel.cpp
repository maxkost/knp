/**
 * @file output_channel.cpp
 * @brief Python bindings for OutputChannel.
 * @kaspersky_developer Vartenkov A.
 * @date 04.06.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include "common.h"


#ifdef KNP_IN_BASE_FW
py::class_<knp::framework::io::output::OutputChannel, boost::noncopyable>(
    "OutputChannel", "The OutputChannel class is a definition of an output channel.", py::no_init)
    .def("get_uid", &get_entity_uid<knp::framework::io::output::OutputChannel>, "Get output channel UID.")
    .def(
        "update", &knp::framework::io::output::OutputChannel::update,
        "Unload spike messages from the endpoint into the message buffer.")
    .def(
        "read_some_from_buffer", &knp::framework::io::output::OutputChannel::read_some_from_buffer,
        "Read a specified interval of messages from sorted internal message buffer.")
    .def("__init__", &construct_output_channel, "Initialize output channel attributes.");
#endif  // KNP_IN_BASE_FW
