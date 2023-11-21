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
#include <knp/core/population.h>
#include <knp/core/projection.h>

#include <atomic>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <boost/config.hpp>

/**
 * @brief Core library namespace.
 */
namespace knp::core
{
/**
 * @brief The Backend class is the base class for backends.
 */
class BOOST_SYMBOL_VISIBLE Backend
{
public:
    /**
     * @brief Predicate type.
     * @details If the predicate returns `true`, network execution continues. Otherwise network execution stops./n
     * The predicate gets a step number as a parameter.
     */
    using RunPredicate = std::function<bool(knp::core::messaging::Step)>;

public:
    /**
     * @brief Pure virtual backend destructor.
     */
    virtual ~Backend() = 0;

public:
    /**
     * @brief Get backend UID.
     * @return backend UID.
     */
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }
    /**
     * @brief Get tags used by the backend.
     * @return backend tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * @brief Define if plasticity is supported.
     * @return `true` if plasticity is supported, `false` if plasticity is not supported.
     */
    [[nodiscard]] virtual bool plasticity_supported() const = 0;
    /**
     * @brief Get type names of supported neurons.
     * @return vector of supported neuron type names.
     */
    [[nodiscard]] virtual std::vector<std::string> get_supported_neurons() const = 0;
    /**
     * @brief Get type names of supported synapses.
     * @return vector of supported synapse type names.
     */
    [[nodiscard]] virtual std::vector<std::string> get_supported_synapses() const = 0;
    /**
     * @brief Get indexes of supported populations.
     * @return vector of indexes of supported populations.
     */
    [[nodiscard]] virtual std::vector<size_t> get_supported_population_indexes() const = 0;
    /**
     * @brief Get indexes of supported projections.
     * @return vector of indexes of supported projections.
     */
    [[nodiscard]] virtual std::vector<size_t> get_supported_projection_indexes() const = 0;

public:
    /**
     * @brief Add projections to backend.
     * @throw exception if the `projections` parameters contains unsupported projection types.
     * @param projections projections to add.
     */
    virtual void load_all_projections(const std::vector<AllProjectionsVariant> &projections) = 0;

    /**
     * @brief Add populations to backend.
     * @throw exception if the `populations` parameter contains unsupported population types.
     * @param populations populations to add.
     */
    virtual void load_all_populations(const std::vector<AllPopulationsVariant> &populations) = 0;

    /**
     * @brief Remove projections with given UIDs from the backend.
     * @param uids UIDs of projections to remove.
     */
    virtual void remove_projections(const std::vector<UID> &uids) = 0;

    /**
     * @brief Remove populations with given UIDs from the backend.
     * @param uids UIDs of populations to remove.
     */
    virtual void remove_populations(const std::vector<UID> &uids) = 0;

public:
    /**
     * @brief Get a list of devices supported by the backend.
     * @note Constant method.
     * @return list of devices.
     * @see Device.
     */
    [[nodiscard]] virtual std::vector<std::unique_ptr<Device>> get_devices() const = 0;

    /**
     * @brief Get a list of devices on which the backend runs a network.
     * @return list of devices.
     * @see Device.
     */
    std::vector<std::unique_ptr<Device>> &get_current_devices() { return devices_; }
    /**
     * @brief Get a list of devices on which the backend runs a network.
     * @note Constant method.
     * @return list of devices.
     * @see Device.
     */
    const std::vector<std::unique_ptr<Device>> &get_current_devices() const { return devices_; }

    /**
     * @brief Select devices on which to run the backend.
     * @param uids set of device UIDs that the backend uses.
     */
    virtual void select_devices(const std::set<UID> &uids);

public:
    /**
     * @brief Get message endpoint.
     * @note Constant method.
     * @return message endpoint.
     */
    virtual const core::MessageEndpoint &get_message_endpoint() const = 0;
    /**
     * @brief Get message endpoint.
     * @return message endpoint.
     */
    virtual core::MessageEndpoint &get_message_endpoint() = 0;

public:
    /**
     * @brief Start network execution on the backend.
     */
    void start();
    /**
     * @brief Start network execution on the backend.
     * @param pre_step function to run before the current step.
     * @param post_step function to run after the current step.
     */
    void start(RunPredicate pre_step, RunPredicate post_step);
    /**
     * @brief Start network execution on the backend.
     * @details If the predicate returns `true`, network execution continues. Otherwise network execution stops./n
     * The predicate gets a step number as a parameter.
     * @param run_predicate predicate function.
     */
    void start(RunPredicate run_predicate);

    /**
     * @brief Stop network execution on the backend.
     */
    void stop();

    /**
     * @brief Make one network execution step.
     * @details You can use this method for debugging purposes.
     */
    virtual void step() = 0;

    /**
     * @brief Get current step.
     * @return step number.
     */
    core::messaging::Step get_step() const { return step_; }

    /**
     * @brief Stop learning.
     */
    virtual void lock() = 0;

    /**
     * @brief Restart learning.
     */
    virtual void unlock() = 0;

public:
    /**
     * @brief Get network execution status.
     * @return `true` if network is being executed, `false` if network is not being executed.
     */
    bool running() const { return started_; }

protected:
    /**
     * @brief Backend default constructor.
     */
    Backend() : message_bus_(knp::core::MessageBus::construct_cpu_bus()) {}

    /**
     * @brief Backend constructor with custom message bus implementation.
     * @param message_bus message bus.
     */
    explicit Backend(MessageBus &&message_bus) : message_bus_(std::move(message_bus)) {}

    /**
     * @brief Initialize backend before starting network execution.
     */
    virtual void init() = 0;

    /**
     * @brief Set backend to the uninitialized state.
     */
    void uninit();

    /**
     * @brief Get and increase the number of the  current step.
     * @return step number.
     */
    core::messaging::Step gad_step() { return step_++; }

public:
    /**
     * @brief Message bus used by backend.
     */
    MessageBus message_bus_;

private:
    void pre_start();

private:
    BaseData base_;
    std::atomic<bool> initialized_ = false;
    volatile std::atomic<bool> started_ = false;
    std::vector<std::unique_ptr<Device>> devices_;
    core::messaging::Step step_ = 0;
};

}  // namespace knp::core
