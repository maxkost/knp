"""
Kaspersky Neuromorphic Platform model tests.

Author: A. Vartenkov
Date: 24.10.2024
"""
from knp.base_framework import Network, Model
from knp.core import UID


def test_add_empty_network() -> None:
    network_uid = UID()
    network = Network(network_uid)
    model = Model(network)
    assert model.network.populations_count == 0
    assert model.network.projections_count == 0
    assert model.network.get_uid() == network_uid
