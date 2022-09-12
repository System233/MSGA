cmake_minimum_required(VERSION 3.0)
project(msga C CXX)

add_subdirectory(lib/hde)

set(MSGA_SRC
    include/msga.h
    src/msga.c
    src/error.c
    src/abi/x86_64.c
    src/abi/amd64.c
    src/abi/x86.c
)
add_library(msga SHARED ${MSGA_SRC})
target_include_directories(msga PUBLIC include)
target_link_libraries(msga hde)

add_library(msga_static STATIC ${MSGA_SRC})
target_include_directories(msga_static PUBLIC include)
target_link_libraries(msga_static hde)


add_executable(msga_test tests/test.c)
target_link_libraries(msga_test msga_static)