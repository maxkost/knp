/**
 * @file base_framework.cpp
 * @brief Base C++ framework Python bindings.
 * @author Artiom N.
 * @date 21.02.24
 */

#include "common.h"

BOOST_PYTHON_MODULE(KNP_FULL_LIBRARY_NAME)
{
#define _KNP_IN_BASE_FW

    py::class_<knp::framework::BackendLoader>("BackendLoader")
        .def("load", &knp::framework::BackendLoader::load, "Load backend")
        .def(
            "is_backend", &knp::framework::BackendLoader::is_backend,
            "Check if the specified path points to a backend");

#undef _KNP_IN_BASE_FW
}
