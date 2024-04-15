/**
 * @file highfive.h
 * @brief HighFive library wrapper with pragmas for CLang.
 * @author An. Vartenkov
 * @date 13.04.2024
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
