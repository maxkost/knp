/**
 * @file highfive.h
 * @brief HighFive library wrapper with pragmas for CLang.
 * @kaspersky_support An. Vartenkov
 * @date 13.04.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once
#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wdocumentation"
#endif
#include <highfive/highfive.hpp>
#ifdef __clang__
#    pragma clang diagnostic pop
#endif
