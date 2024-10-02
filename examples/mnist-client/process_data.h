/**
 * @file process_data.h
 * @brief Functions to process a binary file.
 * @date 13.09.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once
#include <filesystem>
#include <vector>


/**
 * @brief Read spike frames from grayscale file.
 * @param path_to_data path to grayscale image dataset file.
 * @param input_size number of pixels per image.
 * @param frames_per_image number of frames used for a given image.
 * @param num_levels number of intensity levels for an image.
 * @param skip how many frames to skip.
 * @return vector of boolean spike frames.
 */
std::vector<std::vector<bool>> read_spikes_from_grayscale_file(
    const std::filesystem::path &path_to_data, size_t input_size, int frames_per_image, int num_levels,
    size_t skip = 0);
