/**
 * @file altai.h
 * @brief Class definitions for AltAI and AltAI_GM devices.
 * @author Liubiakin A.
 * @date
 */

#pragma once

#include <knp/core/device.h>

#include <cstdlib>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/**
 * @namespace knp::devices
 * @brief Namespace for implementation of concrete devices.
 */

/**
 * @brief AltAI device namespace.
 */
namespace knp::devices::altai
{

/**
 * @brief Structure used to define location of input and output ports in a grid of
 * AltAI cores.
 */
struct Port
{
    /**
     * @brief The `port_side` class is an enumeration class that represents sides of port connections to a grid of AltAI
     * cores.
     */
    enum class port_side
    {
        kNorth,
        kSouth,
        kEast,
        kWest
    };

    /**
     * @brief Start core.
     */
    // cppcheck-suppress unusedStructMember
    size_t start_core_;
    /**
     * @brief Port length from start core.
     */
    // cppcheck-suppress unusedStructMember
    size_t port_length_;
    /**
     * @brief Side {kNorth, kSouth, kEast, kWest}.
     */
    // cppcheck-suppress unusedStructMember
    port_side side_;
};

/**
 * @brief Create port of standard size (4) for AltAI grid, located on the north side.
 * @details The function creates a port with standard length and size, which are used on most of existing AltAI devices.
 * @param begin_core position of a core, from which the port starts.
 * @return created port.
 */
Port make_standart_north_port(size_t begin_core);


/**
 * @brief The `AltAI_GM` class is a defintion of an interface to the AltAI golden model device.
 */
class AltAI_GM : public knp::core::Device
{
public:
    /**
     * @brief Avoid copy of a `AltAI_GM` device.
     */
    AltAI_GM(const AltAI_GM &) = delete;

    /**
     * @brief Avoid copy assignment of a `AltAI_GM` device.
     */
    AltAI_GM &operator=(const AltAI_GM &) = delete;

    /**
     * @brief `AltAI_GM` device move constructor.
     */
    AltAI_GM(AltAI_GM &&);

    /**
     * @brief `AltAI_GM` device move operator.
     * @return reference to `AltAI_GM` instance.
     */
    AltAI_GM &operator=(AltAI_GM &&) noexcept;

    /**
     * @brief `AltAI_GM` device destructor.
     */
    ~AltAI_GM() override;

public:
    /**
     * @brief Get device type.
     * @return device type.
     */
    [[nodiscard]] knp::core::DeviceType get_type() const override;

    /**
     * @brief Get device name.
     * @return device name in the arbitrary format.
     */
    [[nodiscard]] const std::string &get_name() const override;

    /**
     * @brief Get power consumption details for the device.
     * @return amount of consumed power.
     */
    [[nodiscard]] float get_power() const override;

private:
    /**
     * @brief `AltAI_GM` device constructor.
     */
    AltAI_GM();
    friend std::vector<std::unique_ptr<knp::core::Device>> list_altai_devices();

private:
    // cppcheck-suppress unusedStructMember
    std::string altai_name_;
};


/**
 * @brief The `AltAI` class is a definition of an interface to AltAI hardware device.
 */
class AltAI : public knp::core::Device
{
public:
    /**
     * @brief Avoid copy of a AltAI device.
     */
    AltAI(const AltAI &) = delete;

    /**
     * @brief Avoid copy assignment of a AltAI device.
     */
    AltAI &operator=(const AltAI &) = delete;

    /**
     * @brief AltAI device move constructor.
     */
    AltAI(AltAI &&);

    /**
     * @brief AltAI device move operator.
     * @return reference to AltAI instance.
     */
    AltAI &operator=(AltAI &&) noexcept;

    /**
     * @brief AltAI device destructor.
     */
    ~AltAI() override;

public:
    /**
     * @brief Get device type.
     * @return device type.
     */
    [[nodiscard]] knp::core::DeviceType get_type() const override;

    /**
     * @brief Get device name.
     * @return device name in the arbitrary format.
     */
    [[nodiscard]] const std::string &get_name() const override;

    /**
     * @brief Get power consumption details for the device.
     * @return amount of consumed power.
     */
    [[nodiscard]] float get_power() const override;

    /**
     * @brief Get number of columns and rows in device grid.
     * @details Only AltAI hardware device can return this parameters.
     * @return pair of AltAI grid columns and rows.
     */
    [[nodiscard]] std::pair<size_t, size_t> get_grid_cols_rows() const;

    /**
     * @brief Get port vector of device grid.
     * @details Only AltAI hardware device can return this parameters.
     * @return `Port` vector of a given AltAI device.
     */
    [[nodiscard]] std::vector<Port> get_grid_ports() const;

private:
    /**
     * @brief AltAI device constructor.
     */
    AltAI();
    friend std::vector<std::unique_ptr<knp::core::Device>> list_altai_devices();

private:
    // cppcheck-suppress unusedStructMember
    std::string altai_name_;
    // cppcheck-suppress unusedStructMember
    size_t columns_;
    // cppcheck-suppress unusedStructMember
    size_t rows_;
    // cppcheck-suppress unusedStructMember
    std::vector<Port> ports_;
};

/**
 * @brief List all AltAI devices on which backend can be initializated.
 * @return Vector of unique pointers to devices, which are `AltAI` or `AltAI_GM` devices.
 */
std::vector<std::unique_ptr<knp::core::Device>> list_altai_devices();
}  // namespace knp::devices::altai
