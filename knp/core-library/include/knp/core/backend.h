/**
 * @file backend.h
 * @brief Class definition for backend base.
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
 * @namespace knp::backends
 * @brief Namespace for the concrete backends implementation.
 */

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
    using RunPredicate = std::function<bool(knp::core::Step)>;

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
    [[nodiscard]] std::vector<std::unique_ptr<Device>> &get_current_devices() { return devices_; }
    /**
     * @brief Get a list of devices on which the backend runs a network.
     * @note Constant method.
     * @return list of devices.
     * @see Device.
     */
    [[nodiscard]] const std::vector<std::unique_ptr<Device>> &get_current_devices() const { return devices_; }

    /**
     * @brief Select devices on which to run the backend.
     * @param uids set of device UIDs that the backend uses.
     */
    virtual void select_devices(const std::set<UID> &uids);

public:
    /**
     * @brief Subscribe internal endpoint to messages.
     * @details The method is used to get a subscription necessary for receiving messages of the specified type.
     * @tparam MessageType message type.
     * @param receiver receiver UID.
     * @param senders list of possible sender UIDs.
     * @return subscription.
     */
    template <typename MessageType>
    Subscription<MessageType> &subscribe(const UID &receiver, const std::vector<UID> &senders)
    {
        return message_endpoint_.subscribe<MessageType>(receiver, senders);
    }

    /**
     * @brief Message bus used by backend.
     */
    [[nodiscard]] MessageBus &get_message_bus() { return message_bus_; }
    /**
     * @brief Message bus used by backend.
     * @note Constant method.
     */
    [[nodiscard]] const MessageBus &get_message_bus() const { return message_bus_; }

    /**
     * @brief Get message endpoint.
     * @note Constant method.
     * @return message endpoint.
     */
    [[nodiscard]] virtual const core::MessageEndpoint &get_message_endpoint() const { return message_endpoint_; }
    /**
     * @brief Get message endpoint.
     * @return message endpoint.
     */
    [[nodiscard]] virtual core::MessageEndpoint &get_message_endpoint() { return message_endpoint_; }

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
    void start(const RunPredicate &pre_step, const RunPredicate &post_step);
    /**
     * @brief Start network execution on the backend.
     * @details If the predicate returns `true`, network execution continues. Otherwise network execution stops./n
     * The predicate gets a step number as a parameter.
     * @param run_predicate predicate function.
     */
    void start(const RunPredicate &run_predicate);

    /**
     * @brief Stop network execution on the backend.
     */
    void stop();

    /**
     * @brief Get current step.
     * @return step number.
     */
    [[nodiscard]] core::Step get_step() const { return step_; }

    /**
     * @brief Stop learning.
     */
    virtual void stop_learning() = 0;

    /**
     * @brief Restart learning.
     */
    virtual void start_learning() = 0;

public:
    /**
     * @brief Get network execution status.
     * @return `true` if network is being executed, `false` if network is not being executed.
     */
    [[nodiscard]] bool running() const { return started_; }

public:
    /**
     * @brief Initialize backend before starting network execution.
     */
    virtual void _init() = 0;

    /**
     * @brief Make one network execution step.
     * @details You can use this method for debugging purposes.
     */
    virtual void _step() = 0;

    /**
     * @brief Set backend to the uninitialized state.
     */
    virtual void _uninit();

protected:
    /**
     * @brief Backend default constructor.
     */
    Backend();

    /**
     * @brief Backend constructor with custom message bus implementation.
     * @param message_bus message bus.
     */
    explicit Backend(MessageBus &&message_bus);

    /**
     * @brief Get and increase the number of the  current step.
     * @return step number.
     */
    core::Step gad_step() { return step_++; }

private:
    void pre_start();

private:
    BaseData base_;
    std::atomic<bool> initialized_ = false;
    volatile std::atomic<bool> started_ = false;
    std::vector<std::unique_ptr<Device>> devices_;
    MessageBus message_bus_;
    MessageEndpoint message_endpoint_;
    core::Step step_ = 0;
};

}  // namespace knp::core
