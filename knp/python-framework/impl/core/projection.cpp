/**
 * @file projection.cpp
 * @brief Python bindings for Projection.
 * @author Artiom N.
 * @date 16.02.2024
 */

#include "projection.h"

#include <tuple>
#include <vector>

#include "common.h"


//  "Construct an empty projection.")
// "Construct a projection by running a synapse generator a given number of times."
// py::arg("presynaptic_uid"), py::arg("postsynaptic_uid"))

#if defined(_KNP_IN_CORE)

namespace st = knp::synapse_traits;

#    define INSTANCE_PY_PROJECTIONS(n, template_for_instance, synapse_type)                                            \
        py::class_<core::Projection<st::synapse_type>>(                                                                \
            BOOST_PP_STRINGIZE(                                             \
                BOOST_PP_CAT(synapse_type, Projection)),                                                               \
                "The Projection class is a definition of similar connections between the neurons of two populations.", \
                py::no_init)                                                                                           \
                .def(py::init<core::UID, core::UID>())                                                                 \
                .def(py::init<core::UID, core::UID, core::UID>())                                                      \
                .def(                                                                                                  \
                    "__init__",                                                                                        \
                    py::make_constructor(static_cast<std::shared_ptr<core::Projection<st::synapse_type>> (*)(          \
                                             core::UID, core::UID, core::UID, const py::object &, size_t)>(            \
                        &projection_constructor_wrapper<st::synapse_type>)))                                           \
                .def(                                                                                                  \
                    "__init__",                                                                                        \
                    py::make_constructor(static_cast<std::shared_ptr<core::Projection<st::synapse_type>> (*)(          \
                                             core::UID, core::UID, const py::object &, size_t)>(                       \
                        &projection_constructor_wrapper<st::synapse_type>)))                                           \
                .def(                                                                                                  \
                    "add_synapses", &projection_synapses_add_wrapper<st::synapse_type>,                                \
                    "Append connections to the existing projection.")                                                  \
                .def(                                                                                                  \
                    "add_synapses",                                                                                    \
                    static_cast<size_t (core::Projection<st::synapse_type>::*)(                                        \
                        const std::vector<core::Projection<st::synapse_type>::Synapse> &)>(                            \
                        &core::Projection<st::synapse_type>::add_synapses),                                            \
                    "Add a set of user-defined synapses to the projection.")                                           \
                .def(                                                                                                  \
                    "remove_synapses", &core::Projection<st::synapse_type>::remove_synapses,                           \
                    "Remove synapses with the given indexes from the projection.")                                     \
                .def(                                                                                                  \
                    "__iter__",                                                                                        \
                    py::range(                                                                                         \
                        static_cast<std::vector<core::Projection<st::synapse_type>::Synapse>::iterator (               \
                            core::Projection<st::synapse_type>::*)()>(&core::Projection<st::synapse_type>::begin),     \
                        static_cast<std::vector<core::Projection<st::synapse_type>::Synapse>::iterator (               \
                            core::Projection<st::synapse_type>::*)()>(&core::Projection<st::synapse_type>::end)),      \
                    "Get an iterator of the population.")                                                              \
                .def("__len__", &core::Projection<st::synapse_type>::size);  // NOLINT

/*
                                 //   .def(__getitem__, "Get parameter values of a synapse with the given
   index.")
                                 //   .def(__getitem__, "Get parameter values of a synapse with the given
   index.")



                                      .def("get_shared_parameters",
                                           (core::SharedSynapseParameters(core::Projection::*)()) &
                                           core::Projection::get_shared_parameters,
                                           "Get parameters shared between all synapses.")
                                      .def("get_shared_parameters",
                                           (core::SharedSynapseParameters(core::Projection::*)()) &
                                           core::Projection::get_shared_parameters,
                                           "Get parameters shared between all synapses.")
                                  */

/*
py::class_<core::Synapse>(
    "Synapse",
    "Synapse description structure that contains synapse parameters and indexes of the associated neurons.")

    py::class_<core::SharedSynapseParametersT>(
        "SharedSynapseParametersT", "Shared synapses parameters for the non-STDP variant of the
projection.")

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
                    SharedSynapseParametersT<core::synapse_traits::STDP<Rule,
SynapseT>>::ProcessingType::1);
*/

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_PY_PROJECTIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_SYNAPSES))

#endif
