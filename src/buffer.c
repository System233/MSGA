/**
 * Copyright (c) 2023 System233
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "msga_buffer.h"
#include "utils.h"

#include <malloc.h>
#include <string.h>

MSGA_ERR msga_buffer_init(msga_buffer_t*buffer,int size){
    MSGA_ERROR_BEGIN;
    buffer->size=size;
    if(buffer->size>MSGA_BUFFER_SIZE){
        buffer->ptr=malloc(size);
        MSGA_TEST(buffer->ptr,MSGA_ERR_MALLOC_FAIL);
    }
    MSGA_ERROR_END;
}
void msga_buffer_free(msga_buffer_t*buffer){
    MSGA_ERROR_BEGIN;
    if(buffer->size>MSGA_BUFFER_SIZE){
        free(buffer->ptr);
        buffer->ptr=NULL;
    }
    buffer->size=0;
    MSGA_ERROR_END;
}
msga_uint8_t* msga_buffer_data(msga_buffer_t*buffer){
    if(buffer->size>MSGA_BUFFER_SIZE){
        return buffer->ptr;
    }
    return buffer->data;
}
int msga_buffer_size(msga_buffer_t*buffer){
    return buffer->size;
}
int msga_buffer_cmp(msga_buffer_t*buffer1,msga_buffer_t*buffer2){
    if(buffer1->size!=buffer2->size){
        return buffer1->size>buffer2->size?1:-1;
    }
    return memcmp(msga_buffer_data(buffer1),msga_buffer_data(buffer2),buffer1->size);
}