/**
 * @file altai.cpp
 * @brief Implementation of AltAI devices classes.
 * @author Liubiakin A.
 * @date
 */

#include <knp/devices/altai.h>

#include <boost/uuid/name_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace knp::devices::altai
{

static bool is_generated = false;
static boost::uuids::uuid altai_gm_uid;

static constexpr const int compact_board_columns = 16;
static constexpr const int compact_board_rows = 16;
static const std::vector<Port> compact_board_ports{
    Port{4, 4, Port::port_side::kNorth},
};

Port make_standart_north_port(size_t begin_core)
{
    return Port{begin_core, 4, Port::port_side::kNorth};
}


AltAI_GM::AltAI_GM()
{
    altai_name_ = "AltAIv1_GoldenModel";
    if (!is_generated)
    {
        boost::uuids::random_generator generator;
        altai_gm_uid = generator();
        is_generated = true;
    }
    Device::base_.uid_ = knp::core::UID(altai_gm_uid);
}


AltAI_GM::AltAI_GM(AltAI_GM&& other) : altai_name_{std::move(other.altai_name_)} {}


AltAI_GM::~AltAI_GM() {}


AltAI_GM& AltAI_GM::operator=(AltAI_GM&& other) noexcept
{
    altai_name_.swap(other.altai_name_);
    return *this;
}

knp::core::DeviceType AltAI_GM::get_type() const
{
    return knp::core::DeviceType::AltAI1_NPU;
}


const std::string& AltAI_GM::get_name() const
{
    return altai_name_;
}


float AltAI_GM::get_power() const
{
    return 0.;
}

AltAI::AltAI()
{
    altai_name_ = "AltAIv1_Compact_v1.0";
    if (!is_generated)
    {
        boost::uuids::random_generator generator;
        altai_gm_uid = generator();
        is_generated = true;
    }
    Device::base_.uid_ =
        knp::core::UID(boost::uuids::name_generator(knp::core::UID(altai_gm_uid))(altai_name_.c_str()));
    columns_ = compact_board_columns;
    rows_ = compact_board_rows;
    ports_ = compact_board_ports;
}

AltAI::AltAI(AltAI&& other)
    : altai_name_{std::move(other.altai_name_)},
      columns_{other.columns_},
      rows_{other.rows_},
      ports_{std::move(other.ports_)}
{
}


AltAI::~AltAI() {}


AltAI& AltAI::operator=(AltAI&& other) noexcept
{
    altai_name_.swap(other.altai_name_);
    columns_ = other.columns_;
    rows_ = other.rows_;
    ports_.swap(other.ports_);
    return *this;
}

knp::core::DeviceType AltAI::get_type() const
{
    return knp::core::DeviceType::AltAI1_NPU;
}


const std::string& AltAI::get_name() const
{
    return altai_name_;
}


float AltAI::get_power() const
{
    return 0.;
}


std::pair<size_t, size_t> AltAI::get_grid_cols_rows() const
{
    return std::pair<size_t, size_t>(columns_, rows_);
}


std::vector<Port> AltAI::get_grid_ports() const
{
    return ports_;
}

std::vector<std::unique_ptr<knp::core::Device>> list_altai_devices()
{
    std::vector<std::unique_ptr<knp::core::Device>> devices;
    devices.push_back(std::unique_ptr<knp::core::Device>(new AltAI_GM()));
    return devices;
}

}  // namespace knp::devices::altai
