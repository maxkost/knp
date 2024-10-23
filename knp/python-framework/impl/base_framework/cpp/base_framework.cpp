/**
 * @file base_framework.cpp
 * @brief Python bindings for C++ framework.
 * @author Artiom N.
 * @date 21.02.24
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include "common.h"
#include "input_channel_utility.h"
#include "model_utility.h"
#include "network_utility.h"
#include "output_channel_utility.h"


std::shared_ptr<knp::core::Backend> load_backend(cpp_framework::BackendLoader& loader, const py::object& backend_path)
{
    return loader.load(py::extract<std::string>(backend_path)());
}


BOOST_PYTHON_MODULE(KNP_FULL_LIBRARY_NAME)
{
#define KNP_IN_BASE_FW
    // Py_Initialize();

    // auto path_type = py::import("pathlib.Path");

    py::implicitly_convertible<std::string, std::filesystem::path>();
    py::register_ptr_to_python<std::shared_ptr<knp::core::Backend>>();

    py::class_<cpp_framework::BackendLoader>(
        "BackendLoader", "The BackendLoader class is a definition of a backend loader.")
        // py::return_value_policy<py::manage_new_object>()
        .def("load", &cpp_framework::BackendLoader::load, "Load backend")
        .def("load", &load_backend, "Load backend")
        .def(
            "is_backend", &cpp_framework::BackendLoader::is_backend, "Check if the specified path points to a backend");

#include "input_channel.cpp"   // NOLINT
#include "model.cpp"           // NOLINT
#include "network.cpp"         // NOLINT
#include "network_io.cpp"      // NOLINT
#include "observer.cpp"        // NOLINT
#include "output_channel.cpp"  // NOLINT
#undef KNP_IN_BASE_FW
}
