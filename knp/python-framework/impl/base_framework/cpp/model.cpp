//
// Created by an_vartenkov on 04.06.24.
//

#include "common.h"
#ifdef KNP_IN_BASE_FW


py::class_<knp::framework::Model>("Model", py::no_init, "The Model class is a definition of a model.")
    .def("__init__", py::make_constructor(&model_constructor), "Initialize model attributes.")
    .def("add_input_channel", &knp::framework::Model::add_input_channel, "Add an input channel to the network.")
    .def("add_output_channel", &knp::framework::Model::add_output_channel, "Add an output channel to the network."), 
    .def("get_uid", &get_entity_uid<knp::framework::Model>, "Get model UID.")
    // .add_property("tags", &knp::framework::Model::get_tags)
    .add_property("network", &get_network_from_model, &set_network_to_model, "Get network associated with the model.")
    .add_property("input_channels", &get_model_input_channels, "Return all input channels.")
    .add_property("output_channels", &get_model_output_channels, "Return all output channels.");

#endif  // KNP_IN_BASE_FW
