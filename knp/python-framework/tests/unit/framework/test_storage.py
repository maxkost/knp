"""
Kaspersky Neuromorphic Platform single-thread CPU backend tests.

Author: A. Vartenkov
Date: 01.10.2024
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


# Data storage test
def test_data_storage():  # type: ignore[no-untyped-def]
    network, input_uid, output_uid = create_simple_network()
    assert network.populations_count == 1 and network.projections_count == 2


# Save load network test
def save_load_test(_):  # type: ignore[no-untyped-def]
    return
