/**
 * Copyright (c) 2023 System233
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "msga_io.h"

#if defined(_WIN32)
#include "platform/win.c"
#elif defined(__unix__ )
#include "platform/unix.c"
#else
#include "platform/none.c"
#endif

MSGA_ERR msga_init_rt(msga_context_t *ctx, int pid)
{
    MSGA_ERROR_BEGIN;
    memset(ctx, 0, sizeof(msga_context_t));
    MSGA_TEST(impl_open,MSGA_ERR_NOT_IMPLEMENTED);
    ctx->mmap = impl_mmap;
    ctx->mprotect = impl_mprotect;
    ctx->munmap = impl_munmap;
    ctx->read = impl_read;
    ctx->write = impl_write;
    ctx->done = impl_done;
    ctx->user = impl_open(pid);
    ctx->arch = MSGA_ARCH_UNK;
    MSGA_ERROR_END;
}