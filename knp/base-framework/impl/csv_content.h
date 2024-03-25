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

    const auto &get_header() { return header_; }

    void set_header(std::vector<std::string> header)
    {
        header_ = std::move(header);
        for (size_t i = 0; i < header_.size(); ++i) header_index_.insert({header_[i], i});
    }

    void add_row(std::vector<std::string> new_row) { values_.push_back(std::move(new_row)); }

    template <class V>
    V get_value(size_t row, const std::string &col);

    auto get_rc_size() { return std::make_pair(values_.size(), header_.size()); }

    auto begin() { return values_.begin(); }

    auto end() { return values_.end(); }

private:
    std::vector<std::string> header_;
    std::unordered_map<std::string, int> header_index_;
    std::vector<std::vector<std::string>> values_;
};

}  // namespace knp::framework
