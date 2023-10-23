#include <knp/core-library/include/knp/core/synaptic_index.h>

#include <boost/python.hpp>


namespace py = boost::python;


BOOST_PYTHON_MODULE(SynapticIndex)
{
    py::class_<Connection>("Connection", "Connection description structure.")
        .def(py::self == py::self, "Comparison operator.");

    py::class_<Index>("Index", "Index class used for fast synapse search.")
        .def("insert", &Index::insert, "Add connection to the index.");
}
