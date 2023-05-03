// Copyright (c) 2023 System233
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include "msga/types.h"
#include <fstream>


char cmd[0x1000];
char buffer[0x1000];
char const*self;

size_t read_cmd(char const*cmd,int radix=16){
    auto fp=popen(cmd,"r");
    if(fp==NULL){
        abort();
    }
    fgets(buffer, sizeof(buffer), fp);
    pclose(fp);
    return strtoll(buffer,nullptr,radix);
}
#if defined(_WIN32)
size_t get_sym_off(char const*name){
    snprintf(cmd,sizeof(cmd),"objdump -t \"%s\" |grep  \"%s\"|grep -oP \"\\w+\\s+\\w+$\"|grep -oP \"^\\w+\"",self,name);
    return read_cmd(cmd);
}
size_t get_sym_sec(char const*name){
    snprintf(cmd,sizeof(cmd),"objdump -t \"%s\" |grep \"%s\"|grep -oP \"sec\\s+\\K\\d+\"",self,name);
    return read_cmd(cmd,10)-1;
}
size_t get_sec_addr(int index){
    snprintf(cmd,sizeof(cmd),"objdump -h \"%s\"|grep -oP \"^\\s+%d\\s+\\S+\\w+\\s+\\w+\\s+\\K\\w+\"",self,index);
    return read_cmd(cmd);
}
size_t get_sym_addr(char const*name){
    auto off=get_sym_off(name);
    auto index=get_sym_sec(name);
    auto base=get_sec_addr(index);
    return base+off;
}
#else
size_t get_sym_addr(char const*name){
    snprintf(cmd,sizeof(cmd),"readelf -s \"%s\"|grep -i \"%s\"|head -n 1|grep -oP \":\\s\\K\\w+\"",self,name);
    return read_cmd(cmd);
}
#endif

void setup(char const*target){
    self=target;
}