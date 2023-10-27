#include <knp/core/synaptic_index.h>

#include <boost/python.hpp>


namespace py = boost::python;
namespace sa = knp::core::synapse_access;


BOOST_PYTHON_MODULE(SynapticIndex)
{
//    py::class_<sa::Connection>("Connection", "Connection description structure.")
//        .def(py::self == py::self, "Comparison operator.");

    py::class_<sa::Index>("Index", "Index class used for fast synapse search.")
        .def("insert", &sa::Index::insert, "Add connection to the index.");
}
