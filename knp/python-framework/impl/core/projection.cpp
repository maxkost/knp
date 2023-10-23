#include <knp/core-library/include/knp/core/projection.h>

#include <boost/python.hpp>


namespace py = boost::python;


BOOST_PYTHON_MODULE(Projection)
{
    py::class_<Projection>(
        "Projection",
        "The Projection class is a definition of similar connections between the neurons of two populations.")
        .def(py::init<UID, UID>(), "Construct an empty projection.")
        .def(py::init<UID, UID, UID>(), "Construct an empty projection.")
        .def(
            py::init<UID, UID, SynapseGenerator, size_t>(),
            "Construct a projection by running a synapse generator a given number of times.")
        .def(
            py::init<UID, UID, SynapseGenerator1, size_t>(),
            "Construct a projection by running a synapse generator a given number of times.")
        .def(
            py::init<UID, UID, UID, SynapseGenerator1, size_t>(),
            "Construct a projection by running a synapse generator a given number of times.")
        .def(__getitem__, "Get parameter values of a synapse with the given index.")
        .def(__getitem__, "Get parameter values of a synapse with the given index.")
        .def(
            "begin", ([[nodiscard]] auto(Projection::*)()) & Projection::begin,
            "Get an iterator pointing to the first element of the projection.")
        .def(
            "begin", ([[nodiscard]] auto(Projection::*)()) & Projection::begin,
            "Get an iterator pointing to the first element of the projection.")
        .def(
            "end", ([[nodiscard]] auto(Projection::*)()) & Projection::end,
            "Get an iterator pointing to the last element of the projection.")
        .def(
            "end", ([[nodiscard]] auto(Projection::*)()) & Projection::end,
            "Get an iterator pointing to the last element of the projection.")
        .def(
            "add_synapses", (size_t(Projection::*)(size_t, SynapseGenerator)) & Projection::add_synapses,
            "Append connections to the existing projection.")
        .def("add_synapses", &Projection::add_synapses, "Append connections to the existing projection.")
        .def(
            "add_synapses", (size_t(Projection::*)(std::vector<Synapse>)) & Projection::add_synapses,
            "Add a set of user-defined synapses to the projection.")
        .def("add_synapses", &Projection::add_synapses, "Add a set of user-defined synapses to the projection.")
        .def(
            "remove_synapses", &Projection::remove_synapses,
            "Remove synapses with the given indexes from the projection.")
        .def(
            "get_shared_parameters", (SharedSynapseParameters(Projection::*)()) & Projection::get_shared_parameters,
            "Get parameters shared between all synapses.")
        .def(
            "get_shared_parameters", (SharedSynapseParameters(Projection::*)()) & Projection::get_shared_parameters,
            "Get parameters shared between all synapses.");

    py::class_<Synapse>(
        "Synapse",
        "Synapse description structure that contains synapse parameters and indexes of the associated neurons.")

        py::class_<SharedSynapseParametersT>(
            "SharedSynapseParametersT", "Shared synapses parameters for the non-STDP variant of the projection.")

            py::class_<SharedSynapseParametersT<knp::synapse_traits::STDP<Rule, SynapseT>>>(
                "SharedSynapseParametersT<knp::synapse_traits::STDP<Rule,SynapseT>>",
                "Structure for the parameters shared between synapses for STDP.")

                py::enum_<SharedSynapseParametersT<knp::synapse_traits::STDP<Rule, SynapseT>>::ProcessingType>(
                    "ProcessingType")
                    .value(
                        "STDPOnly",
                        SharedSynapseParametersT<knp::synapse_traits::STDP<Rule, SynapseT>>::ProcessingType::0)
                    .value(
                        "STDPAndSpike",
                        SharedSynapseParametersT<knp::synapse_traits::STDP<Rule, SynapseT>>::ProcessingType::1);
}
