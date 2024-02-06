#include "common.h"


#if defined(__KNP_IN_CORE)

py::class_<core::messaging::MessageHeader>("MessageHeader", "Common header for messages.");

#endif
