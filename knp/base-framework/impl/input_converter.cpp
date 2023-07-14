/**
 * @file input_converter.cpp
 * @brief Input converter routines.
 * @author Artiom N.
 * @date 14.07.2023
 */

#include <knp/framework/io/input_converter.h>

// #include <spdlog/spdlog.h>


core::messaging::SpikeData IndexConverter::operator()(std::istream &stream) const
{
    core::messaging::SpikeData result;

    std::string buffer_string;
    std::getline(stream, buffer_string);

    auto iter_first = buffer_string.begin();
    auto iter_second = iter_first;

    while (iter_first < buffer_string.end())
    {
        iter_second = std::find(iter_second + 1, buffer_string.end(), delim_);
        result.push_back(
            std::stoul(buffer_string.substr(iter_first - buffer_string.begin(), iter_second - buffer_string.begin())));
        iter_first = iter_second + 1;
    }
    return result;
}
