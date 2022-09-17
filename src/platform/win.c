// Copyright (c) 2022 System233
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <windows.h>
#include "msga.h"
#include "../utils.h"

static int win_prot(MSGA_MP prot)
{
    switch (prot)
    {
    case MSGA_MP_READWRITE_EXEC:
        return PAGE_EXECUTE_READWRITE;
        break;
    case MSGA_MP_NONE:
        return PAGE_NOACCESS;
        break;
    case MSGA_MP_READ:
        return PAGE_READONLY;
        break;
    case MSGA_MP_WRITE:
    case MSGA_MP_READWRITE:
        return PAGE_READWRITE;
        break;
    case MSGA_MP_READ_EXEC:
        return PAGE_EXECUTE_READ;
        break;
    case MSGA_MP_WRITE_EXEC:
        return PAGE_EXECUTE_READWRITE;
        break;
    default:
        return 0;
    }
}
static HANDLE win_handle(void *user)
{
    if (user != NULL)
    {
        return (HANDLE)user;
    }
    return GetCurrentProcess();
}

static msga_addr_t win_mmap(msga_addr_t addr, int len, MSGA_MP prot, void *user)
{
    return (msga_addr_t)(VirtualAllocEx(win_handle(user), (void *)addr, len, MEM_COMMIT, win_prot(prot)));
}
static MSGA_ERR win_munmap(msga_addr_t addr, int len, void *user)
{
    MSGA_ERROR_BEGIN;
    MSGA_TEST(VirtualFreeEx(win_handle(user), (void *)addr, len, MEM_RELEASE) != 0, MSGA_ERR_SYSTEM);
    MSGA_ERROR_END;
}
static MSGA_ERR win_mprotect(msga_addr_t addr, int len, MSGA_MP prot, void *user)
{
    MSGA_ERROR_BEGIN;
    MSGA_TEST(VirtualProtectEx(win_handle(user), (void *)addr, len, win_prot(prot), NULL) != FALSE, MSGA_ERR_SYSTEM);
    MSGA_ERROR_END;
}
static int win_read(msga_addr_t addr, void *data, int len, void *user)
{
    MSGA_ERROR_BEGIN;
    SIZE_T size = 0;
    MSGA_TEST(ReadProcessMemory(win_handle(user), (void *)addr, data, len, &size) != FALSE, MSGA_ERR_SYSTEM);
    return size;
    MSGA_ERROR_END;
}
static int win_write(msga_addr_t addr, void const *data, int len, void *user)
{
    MSGA_ERROR_BEGIN;
    SIZE_T size = 0;
    MSGA_TEST(WriteProcessMemory(win_handle(user), (void *)addr, data, len, &size) != FALSE, MSGA_ERR_SYSTEM);
    return size;
    MSGA_ERROR_END;
}

#undef MS_ERROR

#define impl_write win_write
#define impl_read win_read
#define impl_mprotect win_mprotect
#define impl_munmap win_munmap
#define impl_mmap win_mmap