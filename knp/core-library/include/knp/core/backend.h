/**
 * @file backend.h
 * @brief Backend base class definition.
 * @author Artiom N.
 * @date 11.01.2023
 */

#pragma once

#include <knp/core/core.h>
#include <knp/core/device.h>
#include <knp/core/message_bus.h>

#include <memory>
#include <set>
#include <vector>

#include <boost/config.hpp>


namespace knp::core
{

/**
 * @brief The Backend class is the base class for backends.
 */
class BOOST_SYMBOL_VISIBLE Backend
{
public:
    /**
     * @brief Get the backend UID.
     * @return UID.
     */
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }
    /**
     * @brief Get tags used by the backend.
     * @return tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * @brief Remove projections with given UIDs from the backend.
     * @param uids UIDs of projections to remove.
     */
    virtual void remove_projections(const std::vector<UID> &uids) = 0;

    /**
     * @brief Remove synapses of the projection with the given UID from the backend.
     * @param projection_uid projection UID.
     * @param indexes indexes of synapses to remove.
     */
    virtual void remove_synapses(const UID &projection_uid, const std::vector<size_t> &indexes) = 0;

    /**
     * @brief Remove populations with given UIDs from the backend.
     * @param uids UIDs of populations remove.
     */
    virtual void remove_populations(const std::vector<UID> &uids) = 0;

    /**
     * @brief Remove neurons of the population with the given UID from the backend.
     * @param population_uid population UID.
     * @param indexes indexes of neurons to remove.
     */
    void remove_neurons(const UID &population_uid, const std::vector<size_t> &indexes);

public:
    /**
     * @brief Get list of devices supported by the backend.
     * @return list of devices.
     * @see Device.
     */
    [[nodiscard]] virtual std::vector<std::unique_ptr<Device>> get_devices() const = 0;

    /**
     * @brief Get a list of devices on which the backend runs a network.
     * @return list of devices.
     * @see Device.
     */
    const std::vector<std::unique_ptr<Device>> &get_current_devices() const { return devices_; }
    std::vector<std::unique_ptr<Device>> &get_current_devices() { return devices_; }

    /**
     * @brief Select devices on which to run the backend.
     * @param uids set of device UIDs that the backend uses.
     */
    virtual void select_devices(const std::set<UID> &uids);

public:
    /**
     * @brief Start network execution on the backend.
     */
    void start();

    /**
     * @brief Stop network execution on the backend.
     */
    void stop();

    /**
     * @brief Make one network execution step.
     * @details You can use this method for debugging purposes.
     */
    virtual void step() = 0;

public:
    MessageBus message_bus_;

private:
    BaseData base_;
    std::vector<std::unique_ptr<Device>> devices_;
};

}  // namespace knp::core
