//
// Created by an_vartenkov on 30.05.24.
//

#include "common.h"

#ifdef KNP_IN_BASE_FW

py::class_<knp::framework::input::DataGenerator>("DataGenerator", "Class to output data");

py::class_<knp::framework::input::InputChannel, boost::noncopyable>("InputChannel", py::no_init)
    .def("get_uid", &get_entity_uid<knp::framework::input::InputChannel>)
    .def("send", &knp::framework::input::InputChannel::send)
    .def("__init__", &construct_input_channel);


#endif  // KNP_IN_BASE_FW
