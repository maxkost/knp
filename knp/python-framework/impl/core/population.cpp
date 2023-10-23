#include <knp/core-library/include/knp/core/population.h>

#include <boost/python.hpp>


namespace py = boost::python;


BOOST_PYTHON_MODULE(Population)
{
    py::class_<Population>("Population", "The Population class is a container of neurons of the same model.")
        .def(py::init<NeuronGenerator, size_t>(), "Construct a population by running a neuron generator.")
        .def(
            py::init<knp::core::UID, NeuronGenerator, size_t>(),
            "Construct a population by running a neuron generator.")
        .def(
            "get_tags", ([[nodiscard]] auto(Population::*)()) & Population::get_tags,
            "Get tags used by the population.")
        .def(
            "get_tags", ([[nodiscard]] auto(Population::*)()) & Population::get_tags,
            "Get tags used by the population.")
        .def(
            "set_neuron_parameters", &Population::set_neuron_parameters,
            "Set parameters for the specific neuron in the population.")
        .def(
            "set_neurons_parameters", &Population::set_neurons_parameters,
            "Set parameters for the specific neuron in the population.")
        .def("remove_neurons", &Population::remove_neurons, "Remove neurons with given indexes from the population.")
        .def("remove_neuron", &Population::remove_neuron, "Remove a specific neuron from the population.")
        .def(__getitem__, "Get parameter values of a neuron with the given index.")
        .def(__getitem__, "Get parameter values of a neuron with the given index.")
        .def(
            "begin", (auto(Population::*)()) & Population::begin,
            "Get an iterator pointing to the first element of the population.")
        .def(
            "begin", (auto(Population::*)()) & Population::begin,
            "Get an iterator pointing to the first element of the population.")
        .def(
            "end", (auto(Population::*)()) & Population::end,
            "Get an iterator pointing to the last element of the population.")
        .def(
            "end", (auto(Population::*)()) & Population::end,
            "Get an iterator pointing to the last element of the population.");
}
