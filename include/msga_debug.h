// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#ifndef _MSGA_DEBUG_H
#define _MSGA_DEBUG_H
#include "msga_type.h"
#include "msga_hook.h"

#if !defined(NDEBUG)
#include <stdio.h>
#define MSGA_LOG(fmt, ...) fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else
#define MSGA_LOG(fmt, ...)
#endif

MSGA_EXTERN char const *msga_error(MSGA_ERR err);
MSGA_EXTERN void msga_hook_debug(msga_hook_t *hook);

#endif //_MSGA_DEBUG_H