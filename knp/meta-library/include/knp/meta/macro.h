/**
 * @file macro.h
 * @brief Some wide used macros.
 * @author Artiom N.
 * @date 01.03.2024
 */

#pragma once

#include <boost/predef.h>


#if defined(__GNUC__)
#    define KNP_UNROLL_LOOP() _Pragma(BOOST_PP_STRINGIZE(GCC unroll 16))
#elif defined(__clang__)
#    define KNP_UNROLL_LOOP() _Pragma("unroll")
#elif defined(_MSC_VER)
#    define KNP_UNROLL_LOOP() _Pragma("loop(hint_parallel(16))")
#endif
