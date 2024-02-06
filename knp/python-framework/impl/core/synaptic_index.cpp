#include "common.h"

#if defined(__KNP_IN_CORE)
namespace py = boost::python;
namespace sa = knp::core::synapse_access;


//    py::class_<sa::Connection>("Connection", "Connection description structure.")
//        .def(py::self == py::self, "Comparison operator.");

py::class_<sa::Index>("SynapticIndex", "Index class used for fast synapse search.")
    .def("insert", &sa::Index::insert, "Add connection to the index.");
#endif
