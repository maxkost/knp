#include <knp/core-library/include/knp/core/messaging/spike_message.h>

#include <boost/python.hpp>


namespace py = boost::python;


BOOST_PYTHON_MODULE(SpikeMessage)
{
    py::class_<SpikeMessage>("SpikeMessage", "Structure of the spike message.")
}
