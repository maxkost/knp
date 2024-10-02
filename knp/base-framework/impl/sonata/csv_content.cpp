/**
 * @file csv_content.cpp
 * @brief CSV helper file.
 * @author An. Vartenkov
 * @date 22.03.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include "csv_content.h"


namespace knp::framework::sonata
{
CsvContent load_csv_content(const fs::path &csv_path)
{
    if (!is_regular_file(csv_path)) throw std::runtime_error("\"" + csv_path.string() + "\" does not exist.");
    csv2::Reader<
        csv2::delimiter<' '>, csv2::quote_character<'"'>, csv2::first_row_is_header<true>,
        csv2::trim_policy::trim_characters<'\r'>>
        csv_reader;
    csv_reader.mmap(csv_path.string());
    const auto csv_header = csv_reader.header();
    std::vector<std::string> header;
    std::string buf;
    for (const auto &header_cell : csv_header)
    {
        header_cell.read_value(buf);
        header.emplace_back(std::move(buf));
    }
    CsvContent res;
    res.set_header(header);

    for (const auto &row : csv_reader)
    {
        std::vector<std::string> buf_vector;
        buf_vector.reserve(header.size());
        for (const auto &cell : row)
        {
            buf = "";
            cell.read_value(buf);
            buf_vector.push_back(buf);
        }
        // Making sure all vectors are at least required size.
        for (size_t i = buf_vector.size(); i < header.size(); ++i) buf_vector.emplace_back("");
        if (!buf_vector.empty() && !buf_vector[0].empty()) res.add_row(buf_vector);
    }
    return res;
}


void save_csv_content(const CsvContent &csv_data, const fs::path &csv_path)
{
    auto header = csv_data.get_header();
    auto rc_size = csv_data.get_rc_size();
    std::vector<std::vector<std::string>> values;
    values.reserve(rc_size.first);

    for (size_t row = 0; row < rc_size.first; ++row)
    {
        std::vector<std::string> row_values;
        row_values.reserve(rc_size.second);
        std::transform(
            header.begin(), header.end(), std::back_inserter(row_values),
            [row, &csv_data](const std::string &col) { return csv_data.get_value<std::string>(row, col); });
        values.push_back(row_values);
    }
    std::ofstream csv_file(csv_path);
    csv2::Writer<csv2::delimiter<' '>> writer(csv_file);
    writer.write_row(header);
    writer.write_rows(values);
}


template <class V>
V CsvContent::get_value(size_t row, const std::string &col) const
{
    return values_[row][header_index_.find(col)->second];
}


template <>
int CsvContent::get_value<int>(size_t row, const std::string &col) const
{
    return std::stoi(values_[row][header_index_.find(col)->second]);
}
}  // namespace knp::framework::sonata
