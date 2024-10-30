/**
 * @file model_loader.cpp
 * @brief Python bindings for ModelLoader class.
 * @author Vartenkov A.
 * @date 30.10.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */


#include "common.h"

#ifdef KNP_IN_BASE_FW

py::def("make_model_loader", &make_model_loader, "Create model loader.");

py::class_<knp::framework::ModelLoader, boost::noncopyable>(
    "ModelLoader", "The ModelLoader class is a definition of a loader that uploads the model to the specified backend.",
    py::no_init)
    .def("load", &knp::framework::ModelLoader::load, "Write model to backend.");
// TODO: Add input/output channels

#endif  // KNP_IN_BASE_FW
