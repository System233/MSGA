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
    return 0;
}
MSGA_ERR msga_hook_alloc(msga_hook_t *hook)
{
    hook->cmp_len = hook->backup_len > hook->jmp_len ? hook->backup_len : hook->jmp_len;
    hook->buf_len = hook->backup_len + hook->jmp_len + hook->cmp_len;
    hook->buffer = (unsigned char *)malloc(hook->buf_len);
    hook->origin_len = hook->backup_len + MSGA_JMPBUF_SIZE;

    MSGA_TEST(hook->buffer, MSGA_ERR_MALLOC_FAIL);
    hook->origin_addr = msga_mmap(hook->context, 0, hook->origin_len, MSGA_MP_READWRITE_EXEC);
    MSGA_TEST(hook->origin_addr, MSGA_ERR_MMAP);

    hook->backup = hook->buffer;
    hook->jmpbuf = hook->buffer + hook->backup_len;
    hook->cmpbuf = hook->buffer + hook->backup_len + hook->jmp_len;
    return MSGA_ERR_OK;
}
MSGA_ERR msga_hook_free(msga_hook_t *hook, int keep_hook)
{
    if (!keep_hook && hook->origin_addr && hook->origin_len)
    {
        MSGA_CHECK(msga_munmap(hook->context, hook->origin_addr, hook->origin_len));
        hook->origin_addr = 0;
        hook->origin_len = 0;
    }
    if (hook->buffer && hook->buf_len)
    {
        free(hook->buffer);
        hook->cmpbuf = NULL;
        hook->buffer = NULL;
        hook->jmpbuf = NULL;
        hook->backup = NULL;
        hook->cmp_len = 0;
        hook->buf_len = 0;
        hook->jmp_len = 0;
        hook->backup_len = 0;
    }
    return MSGA_ERR_OK;
}

MSGA_ERR msga_dohook(msga_hook_t *hook)
{

    MSGA_CHECK(msga_mprotect(hook->context, hook->target_addr, hook->backup_len, MSGA_MP_READWRITE_EXEC));
    MSGA_TEST(msga_read(hook->context, hook->target_addr, hook->cmpbuf, hook->backup_len) == hook->backup_len, MSGA_ERR_READ_LENGTH_MISMATCH);
    MSGA_TEST(memcmp(hook->cmpbuf, hook->backup, hook->backup_len) == 0, MSGA_ERR_MEMORY_MISMATCH);
    MSGA_TEST(msga_write(hook->context, hook->target_addr, hook->jmpbuf, hook->jmp_len) == hook->jmp_len, MSGA_ERR_WRITE_LENGTH_MISMATCH);
    MSGA_CHECK(msga_mprotect(hook->context, hook->target_addr, hook->jmp_len, MSGA_MP_READ_EXEC));
    return MSGA_ERR_OK;
}

MSGA_ERR msga_unhook(msga_hook_t *hook)
{
    MSGA_CHECK(msga_mprotect(hook->context, hook->target_addr, hook->backup_len, MSGA_MP_READWRITE_EXEC));
    MSGA_TEST(msga_read(hook->context, hook->target_addr, hook->cmpbuf, hook->jmp_len) == hook->jmp_len, MSGA_ERR_READ_LENGTH_MISMATCH);
    MSGA_TEST(memcmp(hook->cmpbuf, hook->jmpbuf, hook->jmp_len) == 0, MSGA_ERR_MEMORY_MISMATCH);
    MSGA_TEST(msga_write(hook->context, hook->target_addr, hook->backup, hook->backup_len) == hook->backup_len, MSGA_ERR_WRITE_LENGTH_MISMATCH);
    MSGA_CHECK(msga_mprotect(hook->context, hook->target_addr, hook->jmp_len, MSGA_MP_READ_EXEC));
    return MSGA_ERR_OK;
}
MSGA_ERR msga_mprotect(msga_context_t *ctx, msga_addr_t addr, int len, int port)
{
    MSGA_TEST(ctx->mprotect, MSGA_ERR_NOT_IMPLEMENTED);
    return ctx->mprotect(addr, len, port, ctx->user);
}
msga_addr_t msga_mmap(msga_context_t *ctx, msga_addr_t addr, int len, int port)
{
    MSGA_TEST(ctx->mmap, 0);
    return ctx->mmap(addr, len, port, ctx->user);
}
MSGA_ERR msga_munmap(msga_context_t *ctx, msga_addr_t addr, int len)
{
    MSGA_TEST(ctx->munmap, MSGA_ERR_NOT_IMPLEMENTED);
    return ctx->munmap(addr, len, ctx->user);
}
int msga_read(msga_context_t *ctx, msga_addr_t addr, void *data, int len)
{
    MSGA_TEST(ctx->read, MSGA_ERR_NOT_IMPLEMENTED);
    return ctx->read(addr, data, len, ctx->user);
}
int msga_write(msga_context_t *ctx, msga_addr_t addr, void const *data, int len)
{
    MSGA_TEST(ctx->write, MSGA_ERR_NOT_IMPLEMENTED);
    return ctx->write(addr, data, len, ctx->user);
}

MSGA_ERR msga_hook_init(msga_context_t *ctx, msga_hook_t *hook, msga_addr_t target_addr, msga_addr_t new_addr, msga_addr_t origin_addr)
{
#if defined(__x86_64__)
    return msga_hook_x64(ctx, hook, target_addr, new_addr, origin_addr);
#elif defined(__i386__)
    return msga_hook_x86(ctx, hook, target_addr, new_addr, origin_addr);
#else
    return MSGA_ERR_NOT_IMPLEMENTED;
#endif
}