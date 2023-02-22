// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#ifndef _MSGA_UTILS_H
#define _MSGA_UTILS_H

#include "msga_type.h"
#define MSGA_ERROR_NAME _msga_error
#define MSGA_ERROR_LABEL _MSGA_ERROR
#define MSGA_ERROR_BEGIN MSGA_ERR MSGA_ERROR_NAME = MSGA_ERR_OK
#define MSGA_ERROR_END          \
    do                          \
    {                           \
        return MSGA_ERR_OK;     \
    MSGA_ERROR_LABEL:           \
        return MSGA_ERROR_NAME; \
    } while (0)
#define MSGA_CHECK(cond)                                                                                                \
    do                                                                                                                  \
    {                                                                                                                   \
        MSGA_ERROR_NAME = (cond);                                                                                       \
        if (MSGA_ERROR_NAME != MSGA_ERR_OK)                                                                             \
        {                                                                                                               \
            MSGA_LOG("%s:%d:%s(): %s(%d)", __FILE__, __LINE__, __func__, msga_error(MSGA_ERROR_NAME), MSGA_ERROR_NAME); \
            goto MSGA_ERROR_LABEL;                                                                                      \
        };                                                                                                              \
    } while (0)

#define MSGA_TEST(COND, ERRNO) \
    do                         \
    {                          \
        if (!(COND))           \
        {                      \
            MSGA_CHECK(ERRNO); \
        };                     \
    } while (0)

#define MSGA_TEST_MEM_RW(COND,EXP_SZ) MSGA_TEST((COND)==EXP_SZ,MSGA_ERR_MEM_RW_FAIL)

#define MSGA_ALIGN(X, A) ((X) + (A)-1) & ~((A)-1)

#define MSGA_GUARD(COND, VAL) \
    if (!(COND))              \
    {                         \
        return VAL;           \
    }

#define CH(value, index) ((((msga_addr_t)(value)) >> (index * 8)) & 0xFF)
#define LB4(value) CH((value),0),CH((value),1),CH((value),2),CH((value),3)
#define LB8(value) LB4(value),LB4(((value)>>32))


#endif //_MSGA_UTILS_H