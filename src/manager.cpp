#include "msga/manager.h"
#include <vector>
#include <algorithm>
bool msga::manager::dohook(hook_t&hook){
    if(!arch().generate(io(),hook.from_addr,hook.to_addr,hook.origin, hook.code,hook.backup,hook.code_ptr)){
        return false;
    }
    hook.co_addr=io().alloc(hook.code.size(),hook.from_addr);
    if(!hook.co_addr){
        return false;
    }
    hook.code.setbase(hook.co_addr);
    hook.code_ptr.setbase(-hook.co_addr);
    io().write(hook.code);
    io().write(hook.origin);
    io().write(hook.code_ptr.get(),hook.origin_addr, hook.code_ptr.size());
    return true;
}
bool msga::manager::unhook(hook_t&hook){
    if(!hook.co_addr){
        return false;
    }
    io().write(hook.backup);
    io().free(hook.co_addr);
    hook.co_addr=0;
    return true;
}

bool msga::manager::dohook(addr_t from,addr_t to, addr_t origin)
{
    if(m_map.count(from)){
        return false;
    }
    auto&list=m_map[from];
    auto it=std::find_if(list.begin(),list.end(),[to](auto&item){return item.to_addr==to;});
    if(it!=list.end()){
        return false;
    }
    hook_t data;
    data.from_addr=from;
    data.to_addr=to;
    data.origin_addr=origin;
    if(dohook(data)){
        list.emplace_front(std::move(data));
    }
    return true;
}

bool msga::manager::rehook(std::list<hook_t>&list,std::list<hook_t>::iterator it,addr_t from,addr_t to){
    if(it==list.end()){
        return false;
    }
    auto&hook=*it;
    if(!unhook(hook)){
        return false;
    }
    if(it->to_addr==to){
        list.erase(it);
        return true;
    }
    if(rehook(list,++it,from,to)){
        return dohook(hook);
    }
    return false;

}
bool msga::manager::unhook(addr_t from,addr_t to)
{
    auto it=m_map.find(from);
    if(it==m_map.end()){
        return false;
    }
    auto&list=m_map[from];
    auto end=std::find_if(list.begin(),list.end(),[to](auto&item){return item.to_addr==to;});
    if(end==list.end()){
        return false;
    }
    return rehook(list,list.begin(),from,to);
}
