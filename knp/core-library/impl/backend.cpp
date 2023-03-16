/**
 * @file backend.cpp
 * @brief Backend base class implementation.
 * @author Artiom N.
 * @date 11.01.2023
 */


#include <knp/core/backend.h>

#include <spdlog/spdlog.h>


namespace knp::core
{

void Backend::select_device(const UID& uid)
{
    SPDLOG_INFO("Finding device with UID = %s", std::string(uid));
    for (auto&& device : get_devices())
    {
        SPDLOG_TRACE("Trying UID %s", std::string(device->get_uid));
        if (device->get_uid() == uid)
        {
            SPDLOG_DEBUG("Device with UID %s was found", std::string(uid));
            device_ = std::move(device);
            return;
        }
    }

    SPDLOG_ERROR("Can't find device with UID = %s", std::string(uid));
    throw std::logic_error("Can't find device with given UID!");
}

}  // namespace knp::core
