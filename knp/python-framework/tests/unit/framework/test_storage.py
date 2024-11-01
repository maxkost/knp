"""
@file test_storage.py
@brief Storage tests.

@kaspersky_support Vartenkov A.
@license Apache 2.0 License.
@copyright © 2024 AO Kaspersky Lab
@date 22.10.2024.

Kaspersky Neuromorphic Platform

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

from knp.base_framework import Network
from knp.core import BLIFATNeuronPopulation, DeltaSynapseProjection, UID
from knp.neuron_traits import BLIFATNeuronParameters
from knp.synapse_traits import DeltaSynapseParameters, OutputType
from typing import Tuple


def create_simple_network() -> Tuple[Network, UID, UID]:
    network = Network()

    def neuron_gen(_):  # type: ignore[no-untyped-def]
        return BLIFATNeuronParameters()

    pop_uid = UID()
    network.add_population(BLIFATNeuronPopulation(pop_uid, neuron_gen, 1))

    def synapse_loop_gen(_):  # type: ignore[no-untyped-def]
        return DeltaSynapseParameters(1.0, 6, OutputType.EXCITATORY), 0, 0

    network.add_projection(DeltaSynapseProjection(pop_uid, pop_uid, synapse_loop_gen, 1))

    def input_projection_gen(_):  # type: ignore[no-untyped-def]
        return DeltaSynapseParameters(1.0, 1, OutputType.EXCITATORY), 0, 0

    input_uid = UID()
    network.add_projection(DeltaSynapseProjection(input_uid, pop_uid, input_projection_gen, 1))
    return network, input_uid, pop_uid
