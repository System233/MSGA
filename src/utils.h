// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include "msga.h"
#define MSGA_CHECK(cond)                                                               \
    do                                                                                 \
    {                                                                                  \
        MSGA_ERR err = (cond);                                                         \
        if (err != MSGA_ERR_OK)                                                        \
        {                                                                              \
            MSGA_LOG("%s:%d:%s(): %s", __FILE__, __LINE__, __func__, msga_error(err)); \
            return err;                                                                \
        };                                                                             \
    } while (0)

#define MSGA_TEST(COND, ERRNO) \
    do                         \
    {                          \
        if (!(COND))           \
        {                      \
            MSGA_CHECK(ERRNO); \
        };                     \
    } while (0)
#define MSGA_ALIGN(X, A) (X + (A)-1) & ~((A)-1)
