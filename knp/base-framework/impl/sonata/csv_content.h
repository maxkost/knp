//
// Created by an_vartenkov on 22.03.24.
//

#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <csv2/reader.hpp>
#include <csv2/writer.hpp>


namespace knp::framework
{
namespace fs = std::filesystem;

class CsvContent
{
public:
    void load(const fs::path &csv_path);

    void save(const fs::path &csv_path);

    const auto &get_header() const { return header_; }

    void set_header(std::vector<std::string> header)
    {
        header_ = std::move(header);
        for (size_t i = 0; i < header_.size(); ++i) header_index_.insert({header_[i], i});
    }

    void add_row(std::vector<std::string> new_row) { values_.push_back(std::move(new_row)); }

    std::vector<std::string> get_row(size_t row_n) { return values_[row_n]; }

    template <class V>
    V get_value(size_t row, const std::string &col);  // TODO const

    auto get_rc_size() const { return std::make_pair(values_.size(), header_.size()); }

    auto begin() { return values_.begin(); }

    auto end() { return values_.end(); }

private:
    std::vector<std::string> header_;
    std::unordered_map<std::string, int> header_index_;
    std::vector<std::vector<std::string>> values_;
};


const std::vector<std::string> edge_file_header = {"edge_type_id", "dynamics_params", "model_template"};
const std::vector<std::string> node_file_header = {"node_type_id", "model_type", "model_template", "model_name"};

}  // namespace knp::framework
