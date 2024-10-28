//
// Created by an_vartenkov on 30.05.24.
//

#include "common.h"

#ifdef KNP_IN_BASE_FW

py::class_<knp::framework::io::input::DataGenerator>(
    "DataGenerator", "The DataGenerator class is a definition of spike message generator.");

py::class_<knp::framework::io::input::InputChannel, boost::noncopyable>(
    "InputChannel", py::no_init)  // , "The InputChannel class is a definition of an input channel.")
    .def("get_uid", &get_entity_uid<knp::framework::io::input::InputChannel>, "Get backend UID.")
    .def(
        "send", &knp::framework::io::input::InputChannel::send,
        "Read data from input stream, form a spike message and send it to an endpoint.");
//.def("__init__", &, "Initialize the input channel attributes.");


#endif  // KNP_IN_BASE_FW
