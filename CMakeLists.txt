cmake_minimum_required(VERSION 3.0)
project(msga C CXX)

add_subdirectory(lib/hde)

set(MSGA_SRC
    include/msga.h
    include/msga_buffer.h
    include/msga_config.h
    include/msga_debug.h
    include/msga_hook.h
    include/msga_type.h
    include/msga_io.h
    # src/msga.c
    src/error.c
    src/buffer.c
    src/io/runtime.c
    src/io/platform/win.c
    src/io/platform/unix.c
    src/io/platform/none.c
    src/io/pe.c
    # src/abi/x86_64.c
    # src/abi/amd64.c
    # src/abi/x86.c
    # src/abi/thumb.c
    # src/abi/aarch32.c
    # src/abi/aarch64.c
)
add_library(msga SHARED ${MSGA_SRC})
target_include_directories(msga PUBLIC include)
target_link_libraries(msga hde)

add_library(msga_static STATIC ${MSGA_SRC})
target_include_directories(msga_static PUBLIC include)
target_link_libraries(msga_static hde)

include_directories(include)
add_executable(msga_test tests/test.c)
add_executable(msga_test_pe tests/pe.c src/io/pe.c)
# target_link_libraries(msga_test_pe/)