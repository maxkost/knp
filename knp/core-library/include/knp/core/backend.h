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
 * @brief Backends base class.
 */
class BOOST_SYMBOL_VISIBLE Backend
{
public:
    /**
     * @brief Get this backend UID.
     * @return UID.
     */
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }
    /**
     * @brief Get this backend tags.
     * @return tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * @brief Remove projections with given UIDs from the backend.
     * @param uids identifiers of the projections, which will be removed.
     */
    virtual void remove_projections(const std::vector<UID> &uids) = 0;

    /**
     * @brief Remove synapses from the projection with given UID from the backend.
     * @param projection_uid identifiers of the projection.
     * @param indexes synapses indexes to remove.
     */
    virtual void remove_synapses(const UID &projection_uid, const std::vector<size_t> &indexes) = 0;

    /**
     * @brief Remove populations with given UIDs from the backend.
     * @param uids identifiers of the populations, which will be removed.
     */
    virtual void remove_populations(const std::vector<UID> &uids) = 0;

    /**
     * @brief Remove neurons from the population with given UID from the backend.
     * @param population_uid identifiers of the population.
     * @param indexes neurons indexes to remove.
     */
    void remove_neurons(const UID &population_uid, const std::vector<size_t> &indexes);

public:
    /**
     * @brief Get devices list, supported by the backend.
     * @return list of the devices.
     * @see Device.
     */
    [[nodiscard]] virtual std::vector<std::unique_ptr<Device>> get_devices() const = 0;

    /**
     * @brief Get device list where back-end runs network.
     * @return list of the devices.
     * @see Device.
     */
    const std::vector<std::unique_ptr<Device>> &get_current_devices() const { return devices_; }
    std::vector<std::unique_ptr<Device>> &get_current_devices() { return devices_; }

    /**
     * @brief Select devices, where backend will work.
     * @param uids vector of the devices which the backend uses.
     */
    virtual void select_devices(const std::set<UID> &uids);

public:
    /**
     * @brief Start execution.
     */
    void start();

    /**
     * @brief Stop execution.
     */
    void stop();

    /**
     * @brief Make one execution step.
     *
     * Need for debugging purposes.
     */
    virtual void step() = 0;

public:
    MessageBus message_bus_;

private:
    BaseData base_;
    std::vector<std::unique_ptr<Device>> devices_;
};

}  // namespace knp::core
