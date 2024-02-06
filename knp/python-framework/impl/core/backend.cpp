#if defined(__KNP_IN_CORE)

/* Abstract class
BOOST_PYTHON_MODULE(Backend)
{
    py::class_<Backend>("Backend", "The Backend class is the base class for backends.")
        .def("load_all_projections", &core::Backend::load_all_projections, "Add projections to backend.")
        .def("load_all_populations", &core::Backend::load_all_populations, "Add populations to backend.")
        .def("remove_projections", &core::Backend::remove_projections, "Remove projections with given UIDs from the
backend.") .def("remove_populations", &core::Backend::remove_populations, "Remove populations with given UIDs from the
backend.") .def( "get_current_devices", (std::vector<std::unique_ptr<Device> >(core::Backend::*)()) &
core::Backend::get_current_devices, "Get a list of devices on which the backend runs a network.") .def(
            "get_current_devices", (std::vector<std::unique_ptr<core::Device> >(core::Backend::*)()) &
core::Backend::get_current_devices, "Get a list of devices on which the backend runs a network.") .def("select_devices",
&core::Backend::select_devices, "Select devices on which to run the backend.") .def( "get_message_endpoint",
(core::MessageEndpoint(core::Backend::*)()) & core::Backend::get_message_endpoint, "Get message endpoint.") .def(
            "get_message_endpoint", (core::MessageEndpoint(core::Backend::*)()) & core::Backend::get_message_endpoint,
            "Get message endpoint.")
        .def("start", (void(core::Backend::*)()) & core::Backend::start, "Start network execution on the backend.")
        .def(
            "start", (void(core::Backend::*)(core::RunPredicate, core::RunPredicate)) & core::Backend::start,
            "Start network execution on the backend.")
        .def("start", (void(core::Backend::*)(core::RunPredicate)) & core::Backend::start, "Start network execution on
the backend.");
}
*/

#endif
