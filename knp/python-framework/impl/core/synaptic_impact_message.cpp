#include <knp/core/messaging/synaptic_impact_message.h>

#include <boost/python.hpp>


namespace py = boost::python;
namespace messaging = knp::core::messaging;


BOOST_PYTHON_MODULE(SynapticImpactMessage)
{
    py::class_<messaging::SynapticImpact>(
        "SynapticImpact",
        "Structure that contains the synaptic impact value and indexes of presynaptic and posynaptic neurons.")
;//        .def(py::self == py::self, "Compare synaptic impact messages.");

    py::class_<messaging::SynapticImpactMessage>("SynapticImpactMessage", "Structure of the synaptic impact message.");
}
