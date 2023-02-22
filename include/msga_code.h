// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#ifndef _MSGA_CODE_H
#define _MSGA_CODE_H
#include "msga_buffer.h"



typedef struct
{
    msga_buffer_t buffer;
    int*reloc_table;
    int reloc_num;
} msga_code_t;

MSGA_EXTERN MSGA_ERR msga_buffer_init(msga_buffer_t*buffer,int size);
MSGA_EXTERN void msga_buffer_free(msga_buffer_t*buffer);
MSGA_EXTERN unsigned char* msga_buffer_data(msga_buffer_t*buffer);
MSGA_EXTERN int msga_buffer_size(msga_buffer_t*buffer);
MSGA_EXTERN int msga_buffer_cmp(msga_buffer_t*buffer1,msga_buffer_t*buffer2);
#endif //_MSGA_CODE_H
