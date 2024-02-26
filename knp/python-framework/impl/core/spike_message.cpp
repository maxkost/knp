/**
 * @file spike_message.cpp
 * @brief SpikeMessage Python bindings.
 * @author Artiom N.
 * @date 01.02.2024
 */
#include "common.h"


#if defined(_KNP_IN_CORE)

// std::shared_ptr<knp::core::messaging::SpikeMessage> sm_constructor(const py::tuple& header, const py::list& spikes)
//{
//     core::messaging::SpikeMessage sm =
//         {
//             {py::extract<core::UID>(header[0]), py::extract<size_t>(header[1])},
//             py_iterable_to_vector<core::messaging::SpikeIndex>(spikes)
//     };
//
//     return std::make_shared<knp::core::messaging::SpikeMessage>(std::move(sm));
// }


py::class_<core::messaging::SpikeData>("SpikeData").def(py::vector_indexing_suite<core::messaging::SpikeData>());
//    .def("__str__", static_cast<std::ostream &(std::ostream &, const
//    core::messaging::SpikeData&)>(&core::messaging::operator<<));


py::class_<core::messaging::SpikeMessage>("SpikeMessage", "Structure of the spike message.")
    //    .def("__init__", py::make_constructor(&sm_constructor))
    .def_readwrite("header", &core::messaging::SpikeMessage::header_, "Message header")
    .def_readwrite(
        "neuron_indexes", &core::messaging::SpikeMessage::neuron_indexes_, "Indexes of the recently spiked neurons")
    .def(py::self_ns::str(py::self));

#endif
