#include <knp/core/messaging/message_header.h>

#include <boost/python.hpp>


namespace py = boost::python;
namespace messaging = knp::core::messaging;


BOOST_PYTHON_MODULE(MessageHeader)
{
    py::class_<messaging::MessageHeader>("MessageHeader", "Common header for messages.");
}
