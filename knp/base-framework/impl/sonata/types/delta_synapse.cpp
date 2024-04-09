/**
 * Saving and loading basic delta synapse.
 */
#include <knp/core/projection.h>
#include <knp/synapse-traits/delta.h>

#include <filesystem>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>

// #include <highfive/highfive.hpp>
#include "../csv_content.h"
#include "../highfive.h"
#include "../save_load_network.h"
#include "type_id_defines.h"


namespace knp::framework::sonata
{

template <>
int get_synapse_type_id<synapse_traits::DeltaSynapse>()
{
    return 1000;
}


template <>
std::string get_synapse_type_name<synapse_traits::DeltaSynapse>()
{
    return "knp:BasicDeltaSynapse";
}


template <>
void add_projection_to_h5<core::Projection<synapse_traits::DeltaSynapse>>(
    HighFive::File &file_h5, const knp::core::Projection<synapse_traits::DeltaSynapse> &projection)
{
    using SynapseParams = synapse_traits::synapse_parameters<synapse_traits::DeltaSynapse>;

    if (!file_h5.exist("edges")) throw std::runtime_error("File doesn't contain \"edges\" group.");

    std::vector<uint64_t> source_ids, target_ids;
    std::vector<decltype(SynapseParams::delay_)> delays;
    std::vector<decltype(SynapseParams::weight_)> weights;
    std::vector<int> out_types;

    source_ids.reserve(projection.size());
    target_ids.reserve(projection.size());
    delays.reserve(projection.size());
    weights.reserve(projection.size());
    out_types.reserve(projection.size());

    for (const auto &v : projection)
    {
        source_ids.push_back(std::get<1>(v));
        target_ids.push_back(std::get<2>(v));
        delays.push_back(std::get<0>(v).delay_);
        weights.push_back(std::get<0>(v).weight_);
        out_types.push_back(static_cast<int>(std::get<0>(v).output_type_));
    }

    HighFive::Group proj_group = file_h5.createGroup("edges/" + std::string(projection.get_uid()));
    HighFive::DataSet source_node_dataset = proj_group.createDataSet("source_node_id", source_ids);
    source_node_dataset.createAttribute("node_population", std::string(projection.get_presynaptic()));

    HighFive::DataSet target_node_dataset = proj_group.createDataSet("target_node_id", target_ids);
    target_node_dataset.createAttribute("node_population", std::string(projection.get_postsynaptic()));

    // Right now we only support one synapse group.
    proj_group.createDataSet("edge_group_id", std::vector(projection.size(), 0));
    proj_group.createDataSet(
        "edge_type_id", std::vector(projection.size(), get_synapse_type_id<synapse_traits::DeltaSynapse>()));

    std::vector<uint64_t> group_index;
    group_index.reserve(projection.size());
    for (size_t i = 0; i < projection.size(); ++i) group_index.push_back(i);

    proj_group.createDataSet("edge_group_index", group_index);

    HighFive::Group syn_group = proj_group.createGroup("0");
    syn_group.createDataSet("syn_weight", weights);
    syn_group.createDataSet("delay", delays);
    syn_group.createDataSet("output_type_", out_types);
    proj_group.createAttribute("is_locked", projection.is_locked());
}


template <>
void add_synapse_type_to_csv<synapse_traits::DeltaSynapse>(const fs::path &csv_path)
{
    CsvContent csv_file;
    if (fs::is_regular_file(csv_path))
    {
        // File already exists, load it.
        csv_file.load(csv_path);
        // Check header correctness.
        auto file_header = csv_file.get_header();
        for (const auto &column_name : edge_file_header)
        {
            if (std::find(file_header.begin(), file_header.end(), column_name) == file_header.end())
                throw std::runtime_error("Couldn't find column: " + column_name + " in file " + csv_path.string());
        }
        // Header is okay, check if type exists already.
        size_t height = csv_file.get_rc_size().first - 1;
        for (size_t row_id = 0; row_id < height; ++row_id)
        {
            int type_id = csv_file.get_value<int>(row_id, "edge_type_id");
            if (type_id == get_synapse_type_id<synapse_traits::DeltaSynapse>())
            {
                return;  // Type exists, nothing to add. TODO: update
            }
        }
        std::vector<std::string> type_row{
            std::to_string(get_synapse_type_id<synapse_traits::DeltaSynapse>()), "",
            get_synapse_type_name<synapse_traits::DeltaSynapse>()};
        csv_file.add_row(type_row);
        csv_file.save(csv_path);
        return;
    }
    // File doesn't exist.
    csv_file.set_header(edge_file_header);
    std::vector<std::string> type_row{
        std::to_string(get_synapse_type_id<synapse_traits::DeltaSynapse>()), "",
        get_synapse_type_name<synapse_traits::DeltaSynapse>()};
    csv_file.add_row(type_row);
    csv_file.save(csv_path);
}
}  // namespace knp::framework::sonata
