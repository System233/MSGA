
#include <stdio.h>

#if defined(_WIN32)
#define RTLD_NOW 0
#include <libloaderapi.h>
void*dlopen(char const*name,int){
    return LoadLibraryA(name);
}
void*dlsym(void*mod,char const*name){
    return (void*)GetProcAddress((HMODULE)mod,name);
}
int dlclose(void*mod){
    return !FreeLibrary((HMODULE)mod);
}
#else
#include <dlfcn.h>
#endif
template<class T>
T*dlsym(void*mod,char const*name){
    return reinterpret_cast<T*>(dlsym(mod,name));
}
#include "test.h"
int main(int argc, char const *argv[])
{
    if(argc!=5){
        SHOW_ARGS(argc,argv);
        fprintf(stderr,"%s <shared library> expect x y\n",argv[0]);
        exit(-1);
    }
    int x=atoi(argv[3]);
    int y=atoi(argv[4]);
    int expect=atoi(argv[3]);
    void*mod=dlopen(argv[1],RTLD_NOW);
    auto*hooked=dlsym<int(int,int)>(mod,"hooked_add");
    auto*test=dlsym<int(int,int)>(mod,"test_add");
    TEST(mod);
    TEST(test);
    TEST(hooked);
    TEST_EQ(expect,test(x,y));
    return 0;
}
