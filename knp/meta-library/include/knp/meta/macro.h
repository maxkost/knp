/**
 * @file macro.h
 * @brief Some wide used macros.
 * @author Artiom N.
 * @date 01.03.2024
 */

#pragma once

#include <boost/predef.h>


#if defined(BOOST_COMP_GNUC)
#    define KNP_UNROLL_LOOP() _Pragma(BOOST_PP_STRINGIZE(GCC unroll 16))
#elif defined(BOOST_COMP_CLANG)
#    define KNP_UNROLL_LOOP() _Pragma("unroll")
#elif defined(BOOST_COMP_MSVC)
// ? #pragma loop(hint_parallel(16)) ?
#    define KNP_UNROLL_LOOP()
#endif
