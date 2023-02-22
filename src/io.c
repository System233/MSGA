/**
 * Copyright (c) 2023 System233
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "msga.h"
#include "msga_utils.h"


MSGA_ERR msga_free(msga_context_t *ctx){
    MSGA_ERROR_BEGIN;
    MSGA_TEST(ctx->done, MSGA_ERR_NOT_IMPLEMENTED);
    MSGA_CHECK(ctx->done(ctx->user));
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
