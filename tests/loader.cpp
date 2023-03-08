
#include <dlfcn.h>
#include <stdio.h>
#include <elf.h>
int main(int argc, char const *argv[])
{
    // void*mod=dlopen("libmsga_dronex.so",RTLD_NOW);
    printf("lib=%s\n",argv[1]);
    void*mod=dlopen(argv[1],RTLD_NOW);
    printf("mod:%p\n",mod);
    auto*hooked=(void(*)(int))dlsym(mod,"hooked");
    auto*test=(void(*)(int))dlsym(mod,"test");
    auto*m=(void(*)(int,char const*[]))dlsym(mod,"main");
    printf("hooked:%p\n",hooked);
    printf("test:%p\n",test);
    printf("mainx:%p\n",main);
    printf("main:%p\n",main);
    if(test)
    test(100);
    // m(argc,argv);
    return 0;
}
