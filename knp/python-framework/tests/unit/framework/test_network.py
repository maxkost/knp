"""
Kaspersky Neuromorphic Platform single-thread CPU backend tests.

Author: A. Vartenkov
Date: 01.10.2024
"""
from knp.base_framework import BackendLoader
from knp.base_framework import Network
from knp.core import BLIFATNeuronPopulation, DeltaSynapseProjection, UID
from knp.neuron_traits import BLIFATNeuronParameters
from knp.synapse_traits import DeltaSynapseParameters


def neuron_generator(_):  # type: ignore[no-untyped-def]
    return BLIFATNeuronParameters()


def synapse_generator(_):  # type: ignore[no-untyped-def]
    return DeltaSynapseParameters(), 0, 0


# Network test
def test_empty_network():  # type: ignore[no-untyped-def]
    BackendLoader()
    network = Network()
    assert network.populations_count == 0
    assert network.projections_count == 0


def test_add_population():  # type: ignore[no-untyped-def]
    network = Network()
    population = BLIFATNeuronPopulation(neuron_generator, 1)
    network.add_population(population)
    assert network.populations_count == 1
    assert network.projections_count == 0


def test_add_projection():  # type: ignore[no-untyped-def]
    network = Network()
    projection = DeltaSynapseProjection(UID(), UID(), synapse_generator, 1)
    network.add_projection(projection)
    assert network.populations_count == 0
    assert network.projections_count == 1


def test_remove_projection():  # type: ignore[no-untyped-def]
    network = Network()
    projection = DeltaSynapseProjection(UID(), UID(), synapse_generator, 5)
    proj_uid = projection.uid
    network.add_projection(projection)
    network.remove_projection(proj_uid)
    assert network.projections_count == 0


def test_remove_population():  # type: ignore[no-untyped-def]
    network = Network()
    population = BLIFATNeuronPopulation(neuron_generator, 5)
    pop_uid = population.uid
    network.add_population(population)
    network.remove_population(pop_uid)
    assert network.populations_count == 0
