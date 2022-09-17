// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <stdio.h>
#include "msga.h"
#include "utils.h"

#if defined(_WIN32) || defined(WIN32)
#include "platform/win.c"
#elif defined(__unix__)
#include "platform/unix.c"
#else
#include "platform/none.c"
#endif
#include <malloc.h>
#include <string.h>

MSGA_ERR msga_init(msga_context_t *ctx, void *user)
{
    memset(ctx, 0, sizeof(msga_context_t));
    ctx->mmap = impl_mmap;
    ctx->mprotect = impl_mprotect;
    ctx->munmap = impl_munmap;
    ctx->read = impl_read;
    ctx->write = impl_write;
    ctx->user = user;
    return MSGA_ERR_OK;
}
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
    MSGA_CHECK(msga_mprotect(hook->context, hook->target_addr, hook->backup_len, MSGA_MP_READWRITE_EXEC));
    MSGA_TEST(msga_read(hook->context, hook->target_addr, hook->cmpbuf, hook->backup_len) == hook->backup_len, MSGA_ERR_READ_LENGTH_MISMATCH);
    MSGA_TEST(memcmp(hook->cmpbuf, hook->backup, hook->backup_len) == 0, MSGA_ERR_MEMORY_MISMATCH);
    MSGA_TEST(msga_write(hook->context, hook->target_addr, hook->jmpbuf, hook->jmpbuf_len) == hook->jmpbuf_len, MSGA_ERR_WRITE_LENGTH_MISMATCH);
    MSGA_CHECK(msga_mprotect(hook->context, hook->target_addr, hook->jmpbuf_len, MSGA_MP_READ_EXEC));
    MSGA_ERROR_END;
}

MSGA_ERR msga_unhook(msga_hook_t *hook)
{
    MSGA_ERROR_BEGIN;
    MSGA_CHECK(msga_mprotect(hook->context, hook->target_addr, hook->backup_len, MSGA_MP_READWRITE_EXEC));
    MSGA_TEST(msga_read(hook->context, hook->target_addr, hook->cmpbuf, hook->jmpbuf_len) == hook->jmpbuf_len, MSGA_ERR_READ_LENGTH_MISMATCH);
    MSGA_TEST(memcmp(hook->cmpbuf, hook->jmpbuf, hook->jmpbuf_len) == 0, MSGA_ERR_MEMORY_MISMATCH);
    MSGA_TEST(msga_write(hook->context, hook->target_addr, hook->backup, hook->backup_len) == hook->backup_len, MSGA_ERR_WRITE_LENGTH_MISMATCH);
    MSGA_CHECK(msga_mprotect(hook->context, hook->target_addr, hook->jmpbuf_len, MSGA_MP_READ_EXEC));
    MSGA_ERROR_END;
}
MSGA_ERR msga_mprotect(msga_context_t *ctx, msga_addr_t addr, int len, MSGA_MP prot)
{
    MSGA_ERROR_BEGIN;
    MSGA_TEST(ctx->mprotect, MSGA_ERR_NOT_IMPLEMENTED);
    MSGA_CHECK(ctx->mprotect(addr, len, prot, ctx->user));
    MSGA_ERROR_END;
}
msga_addr_t msga_mmap(msga_context_t *ctx, msga_addr_t addr, int len, MSGA_MP prot)
{
    MSGA_ERROR_BEGIN;
    MSGA_TEST(ctx->mmap, 0);
    return ctx->mmap(addr, len, prot, ctx->user);
    MSGA_ERROR_END;
}
MSGA_ERR msga_munmap(msga_context_t *ctx, msga_addr_t addr, int len)
{
    MSGA_ERROR_BEGIN;
    MSGA_TEST(ctx->munmap, MSGA_ERR_NOT_IMPLEMENTED);
    return ctx->munmap(addr, len, ctx->user);
    MSGA_ERROR_END;
}
int msga_read(msga_context_t *ctx, msga_addr_t addr, void *data, int len)
{
    MSGA_ERROR_BEGIN;
    MSGA_TEST(ctx->read, MSGA_ERR_NOT_IMPLEMENTED);
    return ctx->read(addr, data, len, ctx->user);
    MSGA_ERROR_END;
}
int msga_write(msga_context_t *ctx, msga_addr_t addr, void const *data, int len)
{
    MSGA_ERROR_BEGIN;
    MSGA_TEST(ctx->write, MSGA_ERR_NOT_IMPLEMENTED);
    return ctx->write(addr, data, len, ctx->user);
    MSGA_ERROR_END;
}

MSGA_ERR msga_hook_init(msga_hook_t *hook, msga_context_t *ctx, msga_addr_t target_addr, msga_addr_t new_addr, msga_addr_t old_addr)
{
    if (hook && ctx && target_addr && new_addr)
    {
        memset(hook, 0, sizeof(msga_hook_t));
        hook->context = ctx;
        hook->new_addr = new_addr;
        hook->target_addr = target_addr;
        hook->old_addr = old_addr;
        return MSGA_ERR_OK;
    }
    return MSGA_ERR_INVALID_ARGUMENTS;
}

_extern MSGA_ERR msga_hook_setup(msga_hook_t *hook)
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