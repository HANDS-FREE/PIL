#ifndef ASSERT_H
#define ASSERT_H
#include <iostream>

namespace pi
{

// main template
template<bool>
struct StaticAssert;

// specialization
template<>
struct StaticAssert<true>
{};

}

#define pi_static_assert(exp) \
{ pi::StaticAssert<((exp) != 0)> StaticAssertFailed; }

#define pi_assert(f)\
    do { \
        if (!f ) { \
            fprintf(stderr, "ERROR (FILE: %s, LINE: %d, FUNC: %s)\n", \
                __FILE__, __LINE__, __FUNCTION__); \
            exit(1); \
        } \
    } while (0);

#define pi_assert2(f,s)\
    do { \
        if (!f ) { \
            fprintf(stderr, "ERROR: %s (FILE: %s, LINE: %d, FUNC: %s)\n", \
                s, __FILE__, __LINE__, __FUNCTION__); \
            exit(1); \
        } \
    } while (0);

#define pi_check_ptr(ptr) pi_assert(ptr)

#if defined(_DEBUG)
    #define pi_assert_dbg(f) pi_assert(f)
    #define pi_assert2_dbg(f,s) pi_assert2(f,s)

    // level 1: error message
    #define pi_dbg_error(...)     pi::dbg_printf(1, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

    // level 2: warning message
    #define pi_dbg_warn(...)     pi::dbg_printf(2, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

    // level 3: information message (default)
    #define pi_dbg_info(...)     pi::dbg_printf(3, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

    // level 4: trace message
    #define pi_dbg_trace(...)     pi::dbg_printf(4, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

    // level 5: normal message
    #define pi_dbg_message(...)     pi::dbg_printf(5, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#else
    #define pi_assert_dbg(f)
    #define pi_assert2_dbg(f,s)
    #define pi_dbg_error(...)
    #define pi_dbg_warn(...)
    #define pi_dbg_info(...)
    #define pi_dbg_trace(...)
    #define pi_dbg_message(...)
#endif

#define pi_unexpected() pi_dbg_error()

#endif // ASSERT_H
