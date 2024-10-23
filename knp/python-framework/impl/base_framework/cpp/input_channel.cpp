//
// Created by an_vartenkov on 30.05.24.
//

#include "common.h"

#ifdef KNP_IN_BASE_FW

py::class_<knp::framework::io::input::DataGenerator>("DataGenerator", "Class to output data");

py::class_<knp::framework::io::input::InputChannel, boost::noncopyable>("InputChannel", py::no_init)
    .def("get_uid", &get_entity_uid<knp::framework::io::input::InputChannel>)
    .def("send", &knp::framework::io::input::InputChannel::send)
    .def("__init__", &construct_input_channel);


#endif  // KNP_IN_BASE_FW
