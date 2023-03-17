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

void Backend::select_devices(const std::set<UID>& uids)
{
    for (auto&& device : get_devices())
    {
        SPDLOG_DEBUG("Trying UID {}", std::string(device->get_uid));
        if (uids.find(device->get_uid()) != uids.end())
        {
            SPDLOG_INFO("Device with UID {} was selected", std::string(device->get_uid()));
            devices_.push_back(std::move(device));
            return;
        }
        else
        {
            SPDLOG_TRACE("Device with UID {} was not selected", std::string(device->get_uid()));
        }
    }

    if (uids.size() != devices_.size())
    {
        throw std::logic_error("Not all device UIDs were selected!");
    }
}

}  // namespace knp::core
