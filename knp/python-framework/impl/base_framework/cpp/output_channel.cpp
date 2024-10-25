//
// Created by an_vartenkov on 04.06.24.
//

#include "common.h"


#ifdef KNP_IN_BASE_FW
py::class_<knp::framework::io::output::OutputChannel, boost::noncopyable>("OutputChannel", py::no_init, "The OutputChannel class is a definition of an output channel.")
    .def("get_uid", &get_entity_uid<knp::framework::io::output::OutputChannel>, "Get backend UID.")
    // .def("get_tags", &knp::framework::io::output::OutputChannel::get_tags)
    .def("update", &knp::framework::io::output::OutputChannel::update, "Unload spike messages from the endpoint into the message buffer.")
    .def("read_some_from_buffer", &knp::framework::io::output::OutputChannel::read_some_from_buffer, "Read a specified interval of messages from sorted internal message buffer.")
    .def("__init__", &construct_output_channel, "Initialize output channel attributes.");
#endif  // KNP_IN_BASE_FW
