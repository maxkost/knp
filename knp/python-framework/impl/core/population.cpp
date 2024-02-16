/**
 * @file population.cpp
 * @brief Python bindings for Population.
 * @author Artiom N.
 * @date 07.02.2024
 */

#include "population.h"

#include <vector>

#include "common.h"


#if defined(__KNP_IN_CORE)

/*        .def(
            "get_tags", (auto(core::Population<knp::neuron_traits::BLIFATNeuron>::*)()) &core::Population::get_tags,
            "Get tags used by the population.")
        .def( \
            "get_tags", (auto(core::Population::*)()) & core::Population::get_tags, \
            "Get tags used by the population.")
*/

//    .def("__getitem__",
//        static_cast<core::Population<neuron_type>::NeuronParameters&(core::Population<neuron_type>::*)(size_t)>(&core::Population<neuron_type>::operator[]),
//         "Get parameter values of a neuron with the given index.")

// .def("get_neurons_parameters", &core::Population<neuron_type>::get_neurons_parameters, "Get parameters of all neurons
// in the population.")

// .def("set_neuron_parameters", static_cast<void (knp::core::Population<neuron_type>::*)(size_t, const
// core::Population<neuron_type>::NeuronParameters&)>(&core::Population<neuron_type>::set_neuron_parameters),
//     "Set parameters for the specific neuron in the population.")

// .def("get_neuron_parameters", &core::Population<neuron_type>::get_neuron_parameters,
// "Get parameters of the specific neuron in the population.")

// py::class_<nt::neuron_parameters<nt::neuron_type>>(BOOST_PP_STRINGIZE(BOOST_PP_CAT(neuron_type, Parameters)));

#    define INSTANCE_PY_POPULATIONS(n, template_for_instance, neuron_type)                                             \
        py::class_<core::Population<nt::neuron_type>>(                                                                 \
            BOOST_PP_STRINGIZE(BOOST_PP_CAT(neuron_type, Population)),                                                 \
                               "The Population class is a container of neurons of the same model.", py::no_init)       \
                .def(py::init<core::Population<nt::neuron_type>::NeuronGenerator, size_t>())                           \
                .def(                                                                                                  \
                    "__init__",                                                                                        \
                    py::make_constructor(static_cast<std::shared_ptr<core::Population<nt::neuron_type>> (*)(           \
                                             const core::UID &, const py::object &, size_t)>(                          \
                        &population_constructor_wrapper<nt::neuron_type>)))                                            \
                .def(                                                                                                  \
                    "__init__",                                                                                        \
                    py::make_constructor(                                                                              \
                        static_cast<std::shared_ptr<core::Population<nt::neuron_type>> (*)(                            \
                            const py::object &, size_t)>(&population_constructor_wrapper<nt::neuron_type>)))           \
                .def(                                                                                                  \
                    "add_neurons", &population_neurons_add_wrapper<nt::neuron_type>, "Add neurons to the population.") \
                .def(                                                                                                  \
                    "remove_neurons", &core::Population<nt::neuron_type>::remove_neurons,                              \
                    "Remove neurons with given indexes from the population.")                                          \
                .def(                                                                                                  \
                    "remove_neuron", &core::Population<nt::neuron_type>::remove_neuron,                                \
                    "Remove a specific neuron from the population.")                                                   \
                .def(                                                                                                  \
                    "begin",                                                                                           \
                    static_cast<std::vector<core::Population<nt::neuron_type>::NeuronParameters>::iterator (  // NOLINT         \
                        core::Population<nt::neuron_type>::*)()>(&core::Population<nt::neuron_type>::begin),           \
                    "Get an iterator pointing to the first element of the population.")                                \
                .def(                                                                                                  \
                    "end",                                                                                             \
                    static_cast<std::vector<core::Population<nt::neuron_type>::NeuronParameters>::iterator (  // NOLINT \
                        core::Population<nt::neuron_type>::*)()>(&core::Population<nt::neuron_type>::end),             \
                    "Get an iterator pointing to the last element of the population.")                                 \
                .def(                                                                                                  \
                    "__len__", make_handler([](const core::Population<nt::neuron_type> &population)                    \
                                            { return population.size(); }));

// cppcheck-suppress unknownMacro
BOOST_PP_SEQ_FOR_EACH(INSTANCE_PY_POPULATIONS, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_NEURONS))
#endif
