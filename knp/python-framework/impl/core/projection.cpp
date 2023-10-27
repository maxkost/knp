#include <knp/core/projection.h>

#include <boost/python.hpp>


namespace py = boost::python;
namespace core = knp::core;

/*
BOOST_PYTHON_MODULE(Projection)
{
    py::class_<core::Projection>(
        "Projection",
        "The Projection class is a definition of similar connections between the neurons of two populations.")
        .def(py::init<core::UID, core::UID>(), "Construct an empty projection.")
        .def(py::init<core::UID, core::UID, core::UID>(), "Construct an empty projection.")
        .def(
            py::init<core::UID, core::UID, core::SynapseGenerator, size_t>(),
            "Construct a projection by running a synapse generator a given number of times.")
        .def(
            py::init<core::UID, core::UID, core::SynapseGenerator1, size_t>(),
            "Construct a projection by running a synapse generator a given number of times.")
        .def(
            py::init<core::UID, core::UID, core::UID, core::SynapseGenerator1, size_t>(),
            "Construct a projection by running a synapse generator a given number of times.")
//        .def(__getitem__, "Get parameter values of a synapse with the given index.")
//        .def(__getitem__, "Get parameter values of a synapse with the given index.")
        .def(
            "begin", (auto(core::Projection::*)()) & core::Projection::begin,
            "Get an iterator pointing to the first element of the projection.")
        .def(
            "begin", (auto(core::Projection::*)()) & core::Projection::begin,
            "Get an iterator pointing to the first element of the projection.")
        .def(
            "end", (auto(core::Projection::*)()) & core::Projection::end,
            "Get an iterator pointing to the last element of the projection.")
        .def(
            "end", (auto(core::Projection::*)()) & core::Projection::end,
            "Get an iterator pointing to the last element of the projection.")
        .def(
            "add_synapses", (size_t(core::Projection::*)(size_t, core::SynapseGenerator)) & core::Projection::add_synapses,
            "Append connections to the existing projection.")
        .def("add_synapses", &Projection::add_synapses, "Append connections to the existing projection.")
        .def(
            "add_synapses", (size_t(core::Projection::*)(std::vector<Synapse>)) & core::Projection::add_synapses,
            "Add a set of user-defined synapses to the projection.")
        .def("add_synapses", &core::Projection::add_synapses, "Add a set of user-defined synapses to the projection.")
        .def(
            "remove_synapses", &core::Projection::remove_synapses,
            "Remove synapses with the given indexes from the projection.")
        .def(
            "get_shared_parameters", (core::SharedSynapseParameters(core::Projection::*)()) & core::Projection::get_shared_parameters,
            "Get parameters shared between all synapses.")
        .def(
            "get_shared_parameters", (core::SharedSynapseParameters(core::Projection::*)()) & core::Projection::get_shared_parameters,
            "Get parameters shared between all synapses.");

    py::class_<core::Synapse>(
        "Synapse",
        "Synapse description structure that contains synapse parameters and indexes of the associated neurons.")

        py::class_<core::SharedSynapseParametersT>(
            "SharedSynapseParametersT", "Shared synapses parameters for the non-STDP variant of the projection.")

            py::class_<SharedSynapseParametersT<core::synapse_traits::STDP<Rule, SynapseT>>>(
                "SharedSynapseParametersT<knp::synapse_traits::STDP<Rule,SynapseT>>",
                "Structure for the parameters shared between synapses for STDP.")

                py::enum_<SharedSynapseParametersT<core::synapse_traits::STDP<Rule, SynapseT>>::ProcessingType>(
                    "ProcessingType")
                    .value(
                        "STDPOnly",
                        SharedSynapseParametersT<core::synapse_traits::STDP<Rule, SynapseT>>::ProcessingType::0)
                    .value(
                        "STDPAndSpike",
                        SharedSynapseParametersT<core::synapse_traits::STDP<Rule, SynapseT>>::ProcessingType::1);
}
*/
