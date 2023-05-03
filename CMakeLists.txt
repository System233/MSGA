cmake_minimum_required(VERSION 3.1)
project(msga C CXX)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_CROSSCOMPILING True)

add_subdirectory(lib/hde)
set_property(TARGET hde PROPERTY POSITION_INDEPENDENT_CODE ON)
set(MSGA_SRC
    include/msga/io.h
    include/msga/io/pe.h
    src/manager.cpp
    src/io.cpp
    src/code.cpp
    src/arch/x86_64.cpp
    src/arch/ihde.cpp
    src/arch/ihde32.cpp
    src/arch/ihde64.cpp
    src/io/helper.cpp
    src/io/elf.cpp
    src/io/elf_debug.cpp
    src/io/pe.cpp
    src/io/pe_debug.cpp
    src/io/runtime.cpp
)
add_library(msga SHARED ${MSGA_SRC})
target_include_directories(msga PUBLIC include)
target_link_libraries(msga hde)

add_library(msga_static STATIC ${MSGA_SRC})
target_include_directories(msga_static PUBLIC include)
target_link_libraries(msga_static hde)

include_directories(include)

enable_testing()
add_subdirectory(tests)

