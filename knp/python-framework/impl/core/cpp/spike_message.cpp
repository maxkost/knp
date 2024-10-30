/**
 * @file spike_message.cpp
 * @brief Python bindings for SpikeMessage.
 * @kaspersky_support Artiom N.
 * @date 01.02.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if defined(_KNP_IN_CORE)

#    include "spike_message.h"

#    include "common.h"


py::class_<core::messaging::SpikeData>("SpikeData", "List of spike indexes.")
    .def(py::vector_indexing_suite<core::messaging::SpikeData>());
// .def("__str__", static_cast<std::ostream &(std::ostream &, const
// core::messaging::SpikeData&)>(&core::messaging::operator<<));


py::class_<core::messaging::SpikeMessage>("SpikeMessage", "Structure of the spike message.")
    .def("__init__", py::make_constructor(&spike_message_constructor))
    .add_property("header", &core::messaging::SpikeMessage::header_, "Message header")
    .add_property(
        "neuron_indexes", &core::messaging::SpikeMessage::neuron_indexes_, "Indexes of the recently spiked neurons")
    .def(py::self_ns::str(py::self));

#endif
