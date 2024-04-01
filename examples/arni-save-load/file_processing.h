//
// Created by an_vartenkov on 10.11.23.
//

#pragma once
#include <knp/core/uid.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

struct TrainingData
{
    std::vector<size_t> labels_;
    size_t skip_frames_ = 0;
    size_t skip_labels_ = 0;
    size_t period_ = 1;
    knp::core::UID training_channel_uid_{false};
    int index_shift_ = 0;
};

std::vector<size_t> load_ground_truth(const std::filesystem::path &path_to_ground_truth)
{
    std::vector<size_t> result;
    std::ifstream file(path_to_ground_truth);
    if (!file.is_open()) throw std::runtime_error("Couldn't open the file " + path_to_ground_truth.string());
    while (file.good())
    {
        size_t buf;
        file >> buf;
        result.push_back(buf);
    }
    return result;
}


std::vector<size_t> load_ground_truth_adv(const std::filesystem::path &path_to_ground_truth)
{
    std::vector<size_t> result;
    std::ifstream file(path_to_ground_truth);
    if (!file.is_open()) throw std::runtime_error("Couldn't open the file " + path_to_ground_truth.string());
    while (file.good())
    {
        size_t timestamp, value, left, top;
        char comma;
        file >> timestamp >> comma >> value >> comma >> left >> comma >> top;
        result.push_back(value);
    }
    return result;
}
