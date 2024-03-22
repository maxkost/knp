//
// Created by an_vartenkov on 22.03.24.
//

#include "csv_content.h"

namespace knp::framework
{
void CsvContent::load(const fs::path &csv_path)
{
    if (!is_regular_file(csv_path)) throw std::runtime_error(csv_path.string() + " doesn't exist!");

    csv2::Reader<
        csv2::delimiter<' '>, csv2::quote_character<'"'>, csv2::first_row_is_header<true>,
        csv2::trim_policy::trim_whitespace>
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
    set_header(header);

    for (const auto &row : csv_reader)
    {
        std::vector<std::string> buf_vector;
        buf_vector.reserve(header_.size());
        for (const auto &cell : row)
        {
            cell.read_value(buf);
            buf_vector.push_back(buf);
        }
        // Making sure all vectors are at least required size.
        for (size_t i = buf_vector.size(); i < header_.size(); ++i) buf_vector.emplace_back("");

        values_.push_back(buf_vector);
    }
}


void CsvContent::save(const fs::path &csv_path)
{
    std::ofstream csv_file(csv_path);
    csv2::Writer<csv2::delimiter<' '>> writer(csv_file);

    writer.write_row(header_);
    writer.write_rows(values_);
}
}  // namespace knp::framework
