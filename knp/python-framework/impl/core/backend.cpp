#include <knp/core-library/include/knp/core/backend.h>

#include <boost/python.hpp>


namespace py = boost::python;


BOOST_PYTHON_MODULE(Backend)
{
    py::class_<Backend>("Backend", "The Backend class is the base class for backends.")
        .def("load_all_projections", &Backend::load_all_projections, "Add projections to backend.")
        .def("load_all_populations", &Backend::load_all_populations, "Add populations to backend.")
        .def("remove_projections", &Backend::remove_projections, "Remove projections with given UIDs from the backend.")
        .def("remove_populations", &Backend::remove_populations, "Remove populations with given UIDs from the backend.")
        .def(
            "get_current_devices", (std::vector<std::unique_ptr<Device> >(Backend::*)()) & Backend::get_current_devices,
            "Get a list of devices on which the backend runs a network.")
        .def(
            "get_current_devices", (std::vector<std::unique_ptr<Device> >(Backend::*)()) & Backend::get_current_devices,
            "Get a list of devices on which the backend runs a network.")
        .def("select_devices", &Backend::select_devices, "Select devices on which to run the backend.")
        .def(
            "get_message_endpoint", (core::MessageEndpoint(Backend::*)()) & Backend::get_message_endpoint,
            "Get message endpoint.")
        .def(
            "get_message_endpoint", (core::MessageEndpoint(Backend::*)()) & Backend::get_message_endpoint,
            "Get message endpoint.")
        .def("start", (void(Backend::*)()) & Backend::start, "Start network execution on the backend.")
        .def(
            "start", (void(Backend::*)(RunPredicate, RunPredicate)) & Backend::start,
            "Start network execution on the backend.")
        .def("start", (void(Backend::*)(RunPredicate)) & Backend::start, "Start network execution on the backend.");
}
