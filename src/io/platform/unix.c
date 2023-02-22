// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <unistd.h>
// #include <sys/uio.h>
// #include <sys/mman.h>
#include <sys/ptrace.h>
#include <string.h>
#include <stdlib.h>
#include "msga.h"
#include "msga_utils.h"

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
    void *buf = mmap((void *)addr, len, unix_prot(prot), MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buf == MAP_FAILED)
    {
        return 0;
    }
    return (msga_addr_t)buf;
}
static MSGA_ERR unix_munmap(msga_addr_t addr, int len, void *user)
{
    MSGA_ERROR_BEGIN;
    MSGA_TEST(munmap((void *)addr, len) == 0, MSGA_ERR_SYSTEM);
    MSGA_ERROR_END;
}
static MSGA_ERR unix_mprotect(msga_addr_t addr, int len, MSGA_MP prot, void *user)
{
    MSGA_ERROR_BEGIN;
    unix_align(&addr, &len);
    MSGA_TEST(mprotect((void *)addr, len, unix_prot(prot)) == 0, MSGA_ERR_SYSTEM);
    MSGA_ERROR_END;
}
static int unix_read(msga_addr_t addr, void *data, int len, void *user)
{
    fseek(addr,SEEK_SET);
    memcpy(data, (void *)addr, len);
    return len;
}
static int unix_write(msga_addr_t addr, void const *data, int len, void *user)
{
    memcpy((void *)addr, data, len);
    return len;
}
static void unix_done(msga_addr_t addr, void const *data, int len, void *user)
{
    fclose((FILE*)user);
}
static void* unix_open(int pid)
{
    ptrace()
    
    
}

#define impl_open unix_open
#define impl_write unix_write
#define impl_read unix_read
#define impl_mprotect unix_mprotect
#define impl_munmap unix_munmap
#define impl_mmap unix_mmap
#define impl_done unix_done