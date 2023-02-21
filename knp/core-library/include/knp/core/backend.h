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
#include <vector>


namespace knp::core
{

/**
 * @brief Backends base class.
 */
class Backend
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
    [[nodiscard]] virtual std::vector<Device *> &&get_devices() const = 0;

    /**
     * @brief Get device which
     * @return
     */
    virtual std::unique_ptr<Device> &get_current_device() const;

    /**
     * @brief Select device, where backend will work.
     * @param uid of the device which the backend uses.
     */
    void select_device(const UID &uid);

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
};

}  // namespace knp::core
