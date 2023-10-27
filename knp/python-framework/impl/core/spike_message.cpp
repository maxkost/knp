#include <knp/core/messaging/spike_message.h>

#include <boost/python.hpp>


namespace py = boost::python;
namespace messaging = knp::core::messaging;


BOOST_PYTHON_MODULE(SpikeMessage)
{
    py::class_<messaging::SpikeMessage>("SpikeMessage", "Structure of the spike message.");
}
