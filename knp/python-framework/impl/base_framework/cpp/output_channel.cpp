//
// Created by an_vartenkov on 04.06.24.
//

#include "common.h"


#ifdef KNP_IN_BASE_FW
py::class_<knp::framework::io::output::OutputChannel, boost::noncopyable>("OutputChannel", py::no_init)
    .def("get_uid", &get_entity_uid<knp::framework::io::output::OutputChannel>)
    // .def("get_tags", &knp::framework::io::output::OutputChannel::get_tags)
    .def("update", &knp::framework::io::output::OutputChannel::update)
    .def("read_some_from_buffer", &knp::framework::io::output::OutputChannel::read_some_from_buffer)
    .def("__init__", &construct_output_channel);
#endif  // KNP_IN_BASE_FW
