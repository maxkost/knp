//
// Created by an_vartenkov on 04.06.24.
//

#include "common.h"
#ifdef KNP_IN_BASE_FW


py::class_<knp::framework::Model>("Model", py::no_init)
    .def("__init__", py::make_constructor(&model_constructor))
    .def("add_input_channel", &knp::framework::Model::add_input_channel)
    .def("add_output_channel", &knp::framework::Model::add_output_channel)
    .def("get_uid", &get_entity_uid<knp::framework::Model>, "Model UID")
    // .add_property("tags", &knp::framework::Model::get_tags)
    .add_property("network", &get_network_from_model, &set_network_to_model)
    .add_property("input_channels", &get_model_input_channels)
    .add_property("output_channels", &get_model_output_channels);

#endif  // KNP_IN_BASE_FW
