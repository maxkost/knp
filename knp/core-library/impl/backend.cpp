/**
 * @file backend.cpp
 * @brief Backend base class implementation.
 * @author Artiom N.
 * @date 11.01.2023
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */


#include <knp/core/backend.h>

#include <spdlog/spdlog.h>


namespace knp::core
{

Backend::Backend()
    : message_bus_(knp::core::MessageBus::construct_bus()), message_endpoint_{message_bus_.create_endpoint()}
{
}


Backend::Backend(MessageBus&& message_bus)
    : message_bus_(std::move(message_bus)), message_endpoint_{message_bus_.create_endpoint()}
{
}


Backend::~Backend()
{
    stop();
    SPDLOG_INFO("Backend {} unloaded", std::string(base_.uid_));
}


void Backend::pre_start()
{
    if (running())
    {
        return;
    }

    SPDLOG_INFO("Starting backend {}...", std::string(base_.uid_));

    if (!initialized_)
    {
        _init();
        initialized_ = true;
    }

    started_ = true;
}


void Backend::start()
{
    pre_start();

    try
    {
        while (running())
        {
            _step();
        }
    }
    catch (...)
    {
        started_ = false;
        throw;
    }
    SPDLOG_INFO("Backend {} stopped.", std::string(base_.uid_));
}


void Backend::start(const RunPredicate& run_predicate)
{
    pre_start();

    try
    {
        while (running() && run_predicate(step_))
        {
            _step();
        }
    }
    catch (...)
    {
        started_ = false;
        throw;
    }
    SPDLOG_INFO("Backend {} stopped.", std::string(base_.uid_));
}


void Backend::start(const RunPredicate& pre_step, const RunPredicate& post_step)
{
    pre_start();

    try
    {
        while (running())
        {
            if (pre_step && !pre_step(step_))
            {
                break;
            }
            _step();
            if (post_step && !post_step(step_))
            {
                break;
            }
        }
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
    if (!running())
    {
        return;
    }

    SPDLOG_INFO("Stopping backend {}...", std::string(base_.uid_));
    started_ = false;
}


void Backend::_uninit()
{
    if (!initialized_)
    {
        return;
    }
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
