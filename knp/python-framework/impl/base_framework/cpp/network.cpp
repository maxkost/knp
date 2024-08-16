//
// Created by an_vartenkov on 05.06.24.
//
#include "common.h"
// #define KNP_IN_BASE_FW
#ifdef KNP_IN_BASE_FW

// There are different add_projection here:
//    void add_projection(core::AllProjectionsVariant &&projection);
//
//    template <typename ProjectionType>
//    void add_projection(typename std::decay<ProjectionType>::type &&projection);
//
//    template <typename ProjectionType>
//    void add_projection(typename std::decay<ProjectionType>::type &projection);
//
//    template <typename SynapseType>
//    void add_projection(
//        knp::core::UID projection_uid, knp::core::UID pre_population_uid, knp::core::UID post_population_uid,
//        typename knp::core::Projection<SynapseType>::SynapseGenerator1 generator, size_t synapse_count);


#    define ADD_POPULATION_MACRO(n, template_for_instance, neuron_type) \
        .def(                                                           \
            "add_population",                                           \
            &knp::framework::Network::add_population<knp::core::Population<knp::neuron_traits::neuron_type>>)

#    define ADD_PROJECTION_MACRO(n, template_for_instance, synapse_type) \
        .def(                                                            \
            "add_projection",                                            \
            &knp::framework::Network::add_projection<knp::core::Projection<knp::synapse_traits::synapse_type>>)

py::class_<knp::framework::Network>("Network")
    .def(py::init<const knp::core::UID&>())
    .def("add_projection", network_add_projection_variant)
    .def("add_population", network_add_population_variant)
    // BOOST_PP_SEQ_FOR_EACH(ADD_POPULATION_MACRO, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_NEURONS))
    // BOOST_PP_SEQ_FOR_EACH(ADD_PROJECTION_MACRO, "", BOOST_PP_VARIADIC_TO_SEQ(ALL_SYNAPSES))
    // .def("get_population", &knp::framework::Network::get_population)
    // .def("get_projection", &knp::framework::Network::get_projection)
    .def("remove_population", &knp::framework::Network::remove_population)
    .def("remove_projection", &knp::framework::Network::remove_projection)
    .add_property("populations_count", &knp::framework::Network::populations_count)
    .add_property("projections_count", &knp::framework::Network::projections_count)
    .def("get_uid", &get_entity_uid<knp::framework::Network>);


#endif  // KNP_IN_BASE_FW
