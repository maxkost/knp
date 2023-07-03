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

Backend::~Backend()
{
    stop();
    SPDLOG_INFO("Backend {} unloaded", std::string(base_.uid_));
}


void Backend::start()
{
    if (running()) return;

    SPDLOG_INFO("Starting backend {}...", std::string(base_.uid_));

    if (!initialized_)
    {
        init();
        initialized_ = true;
    }

    started_ = true;

    try
    {
        while (running()) step();
    }
    catch (...)
    {
        started_ = false;
        throw;
    }
    SPDLOG_INFO("Backend {} stopped.", std::string(base_.uid_));
}


void Backend::stop()
{
    if (!running()) return;

    SPDLOG_INFO("Stopping backend {}...", std::string(base_.uid_));
    started_ = false;
}


void Backend::uninit()
{
    if (!initialized_) return;
    stop();
    initialized_ = false;
}


void Backend::select_devices(const std::set<UID>& uids)
{
    for (auto&& device : get_devices())
    {
        SPDLOG_DEBUG("Trying UID {}", std::string(device->get_uid()));
        if (uids.find(device->get_uid()) != uids.end())
        {
            SPDLOG_INFO("Device with UID {} was selected", std::string(device->get_uid()));
            devices_.push_back(std::move(device));
            return;
        }
        SPDLOG_TRACE("Device with UID {} was not selected", std::string(device->get_uid()));
    }

    if (uids.size() != devices_.size())
    {
        throw std::logic_error("Not all device UIDs were selected!");
    }
}

}  // namespace knp::core
