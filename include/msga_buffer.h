// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#ifndef _MSGA_BUFFER_H
#define _MSGA_BUFFER_H
#include "msga_config.h"
#include "msga_type.h"
typedef struct
{
    int size;
    union 
    {
        unsigned char *ptr;
        unsigned char data[MSGA_BUFFER_SIZE];
    };
} msga_buffer_t;

MSGA_EXTERN MSGA_ERR msga_buffer_init(msga_buffer_t*buffer,int size);
MSGA_EXTERN void msga_buffer_free(msga_buffer_t*buffer);
MSGA_EXTERN unsigned char* msga_buffer_data(msga_buffer_t*buffer);
MSGA_EXTERN int msga_buffer_size(msga_buffer_t*buffer);
MSGA_EXTERN int msga_buffer_cmp(msga_buffer_t*buffer1,msga_buffer_t*buffer2);
#endif //_MSGA_BUFFER_H
