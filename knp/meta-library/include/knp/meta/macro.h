/**
 * @file macro.h
 * @brief Some wide used macros.
 * @author Artiom N.
 * @date 01.03.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once

#include <boost/predef.h>


#if defined(__GNUC__)
#    define KNP_UNROLL_LOOP() _Pragma("GCC unroll 16")
#elif defined(__clang__)
#    define KNP_UNROLL_LOOP() _Pragma("unroll")
#elif defined(_MSC_VER)
#    define KNP_UNROLL_LOOP() _Pragma("loop(hint_parallel(16))")
#endif
