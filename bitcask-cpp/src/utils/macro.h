#pragma once

#include <assert.h>
import std;

#define RESET "\033[0m"
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"

#define ASSERT( x )                                                            \
    if ( !( x ) ) {                                                            \
        std::println(                                                          \
            "{}Assertion failed, {}In File: {}, {}Function: {}, {}Line: {}.",  \
            RESET, GREEN, __FILE__, YELLOW, __func__, BLUE, __LINE__ );        \
        assert( x );                                                           \
    } else {                                                                   \
        std::println(                                                          \
            "{}Assertion success, {}In File: {}, {}Function: {}, {}Line: {}.", \
            RESET, GREEN, __FILE__, YELLOW, __func__, BLUE, __LINE__ );        \
        assert( x );                                                           \
    }

#define ASSERT_EQ( x, y )                                                      \
    if ( x != y ) {                                                            \
        std::println( "{}Assertion failed, {}In File: {}, {}Function: {}, "    \
                      "{}Line: {}. {}Left: {}, Right:{}",                      \
                      RESET, GREEN, __FILE__, YELLOW, __func__, BLUE,          \
                      __LINE__, RED, x, y );                                   \
        assert( x == y );                                                      \
    } else {                                                                   \
        std::println( "{}Assertion success, {}In File: {}, {}Function: {}, "   \
                      "{}Line: {}. {}Left: {}, Right:{}",                      \
                      RESET, GREEN, __FILE__, YELLOW, __func__, BLUE,          \
                      __LINE__, RED, x, y );                                   \
        assert( x == y );                                                      \
    }
