#pragma once

#include <assert.h>

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
        std::cout << RESET << "Assertion failed, " << GREEN                    \
                  << "In File: " << __FILE__ << ", " << YELLOW                 \
                  << "Function: " << __func__ << ", " << BLUE                  \
                  << "Line: " << __LINE__ << "." << std::endl;                 \
        assert( x );                                                           \
    } else {                                                                   \
        std::cout << RESET << "Assertion success, " << GREEN                   \
                  << "In File: " << __FILE__ << ", " << YELLOW                 \
                  << "Function: " << __func__ << ", " << BLUE                  \
                  << "Line: " << __LINE__ << "." << std::endl;                 \
        assert( x );                                                           \
    }

#define ASSERT_EQ( x, y )                                                      \
    if ( x != y ) {                                                            \
        std::cout << RESET << "Assertion failed, " << GREEN                    \
                  << "In File: " << __FILE__ << ", " << YELLOW                 \
                  << "Function: " << __func__ << ", " << BLUE                  \
                  << "Line: " << __LINE__ << ". " << RED << "Left: " << x      \
                  << ", Right: " << y << std::endl;                            \
        assert( x == y );                                                      \
    } else {                                                                   \
        std::cout << RESET << "Assertion success, " << GREEN                   \
                  << "In File: " << __FILE__ << ", " << YELLOW                 \
                  << "Function: " << __func__ << ", " << BLUE                  \
                  << "Line: " << __LINE__ << ". " << RED << "Left: " << x      \
                  << ", Right:" << y << std::endl;                             \
        assert( x == y );                                                      \
    }
