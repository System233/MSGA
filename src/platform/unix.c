// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <sys/mman.h>
#include <string.h>
#include "msga.h"
static int unix_port(int port)
{
    int p = PROT_NONE;
    if (port & MSGA_MP_READ)
    {
        p |= PROT_READ;
    }
    if (port & MSGA_MP_WRITE)
    {
        p |= PROT_WRITE;
    }
    if (port & MSGA_MP_EXEC)
    {
        p |= PROT_EXEC;
    }
    return p;
}

static msga_addr_t unix_mmap(msga_addr_t addr, int len, int port, void *user)
{
    return (msga_addr_t)mmap((void *)addr, len, unix_port(port), MAP_PRIVATE, 0, 0);
}
static MSGA_ERR unix_munmap(msga_addr_t addr, int len, void *user)
{
    return munmap((void *)addr, len);
}
static MSGA_ERR unix_mprotect(msga_addr_t addr, int len, int port, void *user)
{
    return mprotect((void *)addr, len, unix_port(port));
}
static int unix_read(msga_addr_t addr, void *data, int len, void *user)
{
    memcpy(data, (void *)addr, len);
    return len;
}
static int unix_write(msga_addr_t addr, void const *data, int len, void *user)
{
    memcpy((void *)addr, data, len);
    return len;
}

#define impl_write unix_write
#define impl_read unix_read
#define impl_mprotect unix_mprotect
#define impl_munmap unix_munmap
#define impl_mmap unix_mmap