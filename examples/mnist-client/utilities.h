/**
 * @file utilities.h
 * @brief Utility functions for MNIST.
 * @date 26.07.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */
// TODO: Expand and split.

#pragma once
#include <vector>

#include <opencv2/highgui.hpp>

std::vector<unsigned char> convert_image_to_vector(const cv::Mat &input_img)
{
    std::vector<unsigned char> result;
    result.reserve(input_img.rows * input_img.cols);
    for (size_t y = 0; y < input_img.rows; ++y)
    {
        uchar *row = input_img.row(y).data;
        for (size_t x = 0; x < input_img.cols; ++x)
        {
            result.push_back(row[x]);
        }
    }
    return result;
}


std::vector<std::vector<bool>> spikes_from_linearized_image(std::vector<unsigned char> inputs, size_t num_steps)
{
    std::vector<std::vector<bool>> result(num_steps);
    for (size_t i = 0; i < num_steps; ++i) result[i].resize(inputs.size());

    for (size_t input = 0; input < inputs.size(); ++input)
    {
        double frequency = static_cast<double>(inputs[input]) / num_steps;
        double accumulator = 0.0;
        for (size_t step = 0; step < num_steps; ++step)
        {
            accumulator += frequency;
            if (accumulator > 1.0)
            {
                result[step][input] = true;
                accumulator = 0.0;
            }
        }
    }
    return result;
}
