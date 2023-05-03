// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "msga/arch/x86_64.h"
#include "msga/io.h"
#include "msga/code.h"
#include "msga/types.h"
#include <cstdio>
#include "msga/io/pe.h"
#include "msga/manager.h"
#include "test.h"
using namespace msga::io;
using namespace msga;
#include "setup.h"
int main(int argc, char const *argv[])
{
    if(argc!=6){
        fprintf(stderr,"%s <input> <output> from_sym to_sym origin_sym\n",argv[0]);
        exit(1);
    }
    auto input=argv[1];
    auto output=argv[2];
    setup(input);
    addr_t from=get_sym_addr(argv[3]);
    addr_t to=get_sym_addr(argv[4]);
    addr_t origin=get_sym_addr(argv[5]);
    printf("from:%llx,to:%llx,origin:%llx\n",from,to,origin);
    msga::arch::x86_64 arch;
    msga::io::pe io;
    TEST(io.load(input));
    msga::manager man(arch,io);
    TEST(man.dohook(from,to,origin));
    io.debug();
    TEST(io.dump(output));
    return 0;
}

