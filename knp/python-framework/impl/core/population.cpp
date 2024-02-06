#if defined(__KNP_IN_CORE)

/*
py::class_<core::Population>("Population", "The Population class is a container of neurons of the same model.")
    .def(py::init<core::NeuronGenerator, size_t>(), "Construct a population by running a neuron generator.")
    .def(
        py::init<core::UID, core::NeuronGenerator, size_t>(),
        "Construct a population by running a neuron generator.")
    .def(
        "get_tags", (auto(core::Population::*)()) & core::Population::get_tags,
        "Get tags used by the population.")
    .def(
        "get_tags", (auto(core::Population::*)()) & core::Population::get_tags,
        "Get tags used by the population.")
    .def(
        "set_neuron_parameters", &core::Population::set_neuron_parameters,
        "Set parameters for the specific neuron in the population.")
    .def(
        "set_neurons_parameters", &core::Population::set_neurons_parameters,
        "Set parameters for the specific neuron in the population.")
    .def("remove_neurons", &core::Population::remove_neurons, "Remove neurons with given indexes from the population.")
    .def("remove_neuron", &core::Population::remove_neuron, "Remove a specific neuron from the population.")
//        .def(__getitem__, "Get parameter values of a neuron with the given index.")
//        .def(__getitem__, "Get parameter values of a neuron with the given index.")
    .def(
        "begin", (auto(core::Population::*)()) & core::Population::begin,
        "Get an iterator pointing to the first element of the population.")
    .def(
        "begin", (auto(core::Population::*)()) & core::Population::begin,
        "Get an iterator pointing to the first element of the population.")
    .def(
        "end", (auto(core::Population::*)()) & core::Population::end,
        "Get an iterator pointing to the last element of the population.")
    .def(
        "end", (auto(core::Population::*)()) & core::Population::end,
        "Get an iterator pointing to the last element of the population.");
*/

#endif
