#include <knp/core-library/include/knp/core/messaging/message_header.h>

#include <boost/python.hpp>


namespace py = boost::python;


BOOST_PYTHON_MODULE(MessageHeader)
{
    py::class_<MessageHeader>("MessageHeader", "Common header for messages.")
}
