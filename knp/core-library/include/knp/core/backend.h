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
#include <vector>


namespace knp::core
{

/**
 * @brief Backends base class.
 */
class Backend
{
public:
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /*
     * @brief load populations to the backend.
     * @tparam PopulationType population type, if supported.
     * @param populations vector to load.
     */
    template <typename PopulationType>
    void load_populations(const std::vector<PopulationType> &populations);
    template <typename PopulationType>
    void load_populations(std::vector<PopulationType> &&populations);

    /*
     * @brief load projections to the backend.
     * @tparam ProjectionType projection type, if supported.
     * @param projections vector to load.
     */
    template <typename ProjectionType>
    void load_projections(const std::vector<ProjectionType> &projections);
    template <typename ProjectionType>
    void load_projections(const std::vector<ProjectionType> &&projections);

public:
    void remove_projections(const std::vector<UID> &uids);
    void remove_synapses(const UID &projection_uid, const std::vector<size_t> &indexes);
    void remove_populations(const std::vector<UID> &uids);
    void remove_neurons(const UID &population_uid, const std::vector<size_t> &indexes);

public:
    std::vector<Device> &&get_devices() const;
    Device &get_current_device() const;
    void select_device();

public:
    void start();
    void stop();
    void step();

public:
    MessageBus message_bus_;

private:
    BaseData base_;
};

}  // namespace knp::core
