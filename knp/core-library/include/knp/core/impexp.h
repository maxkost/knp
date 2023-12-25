/**
 * @brief Import and export control header.
 * @author Artiom N.
 * @date 25.10.2021
 */

#pragma once


#if defined(_WIN32) || defined(__CYGWIN__)
#    define _KNP_SYMBOL_EXPORT __attribute__((dllexport))
#    define _KNP_SYMBOL_IMPORT __attribute__((dllimport))
#    define _KNP_SYMBOL_EXTERN extern
#    define _KNP_SYMBOL_LOCAL
#elif defined(__GNUC__) || defined(__clang__)
#    define _KNP_SYMBOL_IMPORT __attribute__((visibility("default")))
#    define _KNP_SYMBOL_EXPORT __attribute__((visibility("default")))
#    define _KNP_SYMBOL_EXTERN
#    define _KNP_SYMBOL_LOCAL __attribute__((visibility("hidden")))
#else
#    define _KNP_SYMBOL_IMPORT
#    define _KNP_SYMBOL_EXPORT
#    define _KNP_SYMBOL_EXTERN
#    define _KNP_SYMBOL_LOCAL
#endif

// Defined if KNP is built as a shared library.
#if defined(BUILD_SHARED_LIBS)
#    define KNP_EXPORT _KNP_SYMBOL_EXPORT
#    define KNP_IMPORT _KNP_SYMBOL_IMPORT
#    define KNP_EXTERN _KNP_SYMBOL_EXTERN
#    define KNP_LOCAL _KNP_SYMBOL_LOCAL
#else
#    define KNP_EXPORT
#    define KNP_IMPORT
#    define KNP_EXTERN
#    define KNP_LOCAL
#endif

#if defined(_KNP_INTERNAL)
#    define KNP_DECLSPEC KNP_EXPORT
#else
#    define KNP_DECLSPEC KNP_IMPORT
#endif
