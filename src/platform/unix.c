// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include "msga.h"
static int unix_prot(MSGA_MP prot)
{
    int p = PROT_NONE;
    if (prot & MSGA_MP_READ)
    {
        p |= PROT_READ;
    }
    if (prot & MSGA_MP_WRITE)
    {
        p |= PROT_WRITE;
    }
    if (prot & MSGA_MP_EXEC)
    {
        p |= PROT_EXEC;
    }
    return p;
}
static void unix_align(msga_addr_t *addr, int *len)
{
    long pagesize = sysconf(_SC_PAGESIZE);
    msga_addr_t end = *addr + *len;
    msga_addr_t base = *addr & ~(pagesize - 1);
    msga_addr_t xlen = end - base;
    *addr = base, *len = (xlen + pagesize - 1) & ~(pagesize - 1);
}
static msga_addr_t unix_mmap(msga_addr_t addr, int len, MSGA_MP prot, void *user)
{
    // unix_align(&addr, &len);
    void *buf = mmap((void *)addr, len, unix_prot(prot), MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (buf == MAP_FAILED)
    {
        return -errno;
    }
    return (msga_addr_t)buf;
}
static MSGA_ERR unix_munmap(msga_addr_t addr, int len, void *user)
{
    return munmap((void *)addr, len);
}
static MSGA_ERR unix_mprotect(msga_addr_t addr, int len, MSGA_MP prot, void *user)
{
    unix_align(&addr, &len);
    return mprotect((void *)addr, len, unix_prot(prot));
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