/**
 * @file synaptic_impact_message.cpp
 * @brief Python bindings for SynapticImpactMessage.
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
#include "common.h"


#if defined(_KNP_IN_CORE)

py::class_<core::messaging::SynapticImpact>(
    "SynapticImpact",
    "Structure that contains the synaptic impact value and indexes of presynaptic and posynaptic neurons.");
// .def(py::self == py::self, "Compare synaptic impact messages.");

py::class_<core::messaging::SynapticImpactMessage>(
    "SynapticImpactMessage", "Structure of the synaptic impact message.");

#endif
