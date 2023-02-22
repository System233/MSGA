// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <stdio.h>
#include "msga.h"
#include "msga_utils.h"

#include <malloc.h>
#include <string.h>

MSGA_ERR msga_hook_alloc(msga_hook_t *hook, int backup_len, int jmpbuf_len, int jmpback_len)
{
    MSGA_ERROR_BEGIN;
    hook->backup_len = backup_len;
    hook->jmpbuf_len = jmpbuf_len;
    hook->cmpbuf_len = hook->backup_len > hook->jmpbuf_len ? hook->backup_len : hook->jmpbuf_len;
    hook->buf_len = hook->backup_len + hook->jmpbuf_len + hook->cmpbuf_len;
    hook->buffer = hook->buf_len > MSGA_BUFFER_SIZE ? (unsigned char *)malloc(hook->buf_len) : hook->_buffer;
    hook->origin_len = hook->backup_len + (jmpback_len ? jmpback_len : MSGA_JMPBACK_SIZE);

    MSGA_TEST(hook->buffer, MSGA_ERR_MALLOC_FAIL);
    hook->origin_addr = msga_mmap(hook->context, 0, hook->origin_len, MSGA_MP_READWRITE_EXEC);
    MSGA_TEST(hook->origin_addr > 0, hook->origin_addr);

    hook->backup = hook->buffer;
    hook->jmpbuf = hook->buffer + hook->backup_len;
    hook->cmpbuf = hook->buffer + hook->backup_len + hook->jmpbuf_len;
    MSGA_ERROR_END;
}
MSGA_ERR msga_hook_free(msga_hook_t *hook, int keep_hook)
{
    MSGA_ERROR_BEGIN;
    if (!keep_hook && hook->origin_addr && hook->origin_len)
    {
        MSGA_CHECK(msga_munmap(hook->context, hook->origin_addr, hook->origin_len));
        hook->origin_addr = 0;
        hook->origin_len = 0;
    }
    if (hook->buffer && hook->buf_len > MSGA_BUFFER_SIZE)
    {
        free(hook->buffer);
        hook->cmpbuf = NULL;
        hook->buffer = NULL;
        hook->jmpbuf = NULL;
        hook->backup = NULL;
        hook->cmpbuf_len = 0;
        hook->buf_len = 0;
        hook->jmpbuf_len = 0;
        hook->backup_len = 0;
    }
    MSGA_ERROR_END;
}

MSGA_ERR msga_dohook(msga_hook_t *hook)
{
    MSGA_ERROR_BEGIN;
    msga_buffer_t cmpbuf;
    int buf_len=msga_buffer_size(&hook->backup);
    MSGA_TEST(msga_buffer_init(&cmpbuf,buf_len));

    MSGA_TEST_MEM_RW(msga_read(hook->context, hook->to_addr, msga_buffer_data(&cmpbuf), buf_len), buf_len);
    MSGA_TEST(msga_buffer_cmp(&cmpbuf, &hook->backup) == 0, MSGA_ERR_MEMORY_MISMATCH);
    MSGA_TEST_MEM_RW(msga_write(hook->context, hook->to_addr, msga_buffer_data(&hook->jmpbuf), buf_len) ,buf_len);
    
    msga_buffer_free(&cmpbuf);
    MSGA_ERROR_END;
}

MSGA_ERR msga_unhook(msga_hook_t *hook)
{
    MSGA_ERROR_BEGIN;
    msga_buffer_t cmpbuf;
    int buf_len=msga_buffer_size(&hook->backup);
    MSGA_TEST(msga_buffer_init(&cmpbuf,buf_len));

    MSGA_TEST_MEM_RW(msga_read(hook->context, hook->to_addr, msga_buffer_data(&cmpbuf), buf_len) ,buf_len);
    MSGA_TEST(msga_buffer_cmp(&cmpbuf, &hook->jmpbuf) == 0, MSGA_ERR_MEMORY_MISMATCH);
    MSGA_TEST_MEM_RW(msga_write(hook->context, hook->to_addr, msga_buffer_data(&hook->backup) , buf_len),buf_len);

    msga_buffer_free(&cmpbuf);
    MSGA_ERROR_END;
}

MSGA_ERR msga_hook_init(msga_hook_t *hook, msga_context_t *ctx, msga_addr_t to_addr, msga_addr_t jmp_addr, msga_addr_t from_addr)
{
    if (hook && ctx && to_addr && jmp_addr)
    {
        memset(hook, 0, sizeof(msga_hook_t));
        hook->context = ctx;
        hook->jmp_addr = jmp_addr;
        hook->to_addr = to_addr;
        hook->from_addr = from_addr;
        return MSGA_ERR_OK;
    }
    return MSGA_ERR_INVALID_ARGUMENTS;
}

MSGA_EXTERN MSGA_ERR msga_hook_setup(msga_hook_t *hook)
{
    if (!hook)
    {
        return MSGA_ERR_INVALID_ARGUMENTS;
    }
#if defined(__x86_64__)
    return msga_hook_setup_x64(hook);
#elif defined(__i386__)
    return msga_hook_setup_x86(hook);
#elif defined(__aarch64__)
    return msga_hook_setup_aarch64(hook);
#elif defined(__arm__)
    return msga_hook_setup_aarch32(hook);
#elif defined(__thumb__)
    return msga_hook_setup_thumb(hook);
#else
    return MSGA_ERR_NOT_IMPLEMENTED;
#endif
}