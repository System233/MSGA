cmake_minimum_required(VERSION 3.0)
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

include_directories(include)
add_executable(msga_hde tests/test_hde.c)
target_link_libraries(msga_hde hde)

add_executable(msga_io_elf tests/test_io_elf.cpp)
target_link_libraries(msga_io_elf msga_static)

add_executable(msga_io_pe tests/test_io_pe.cpp)
target_link_libraries(msga_io_pe msga_static)

if(UNIX)
set(TEST_LIB dl)
endif()
add_executable(msga_drone tests/drone.cpp)
target_link_libraries(msga_drone msga_static ${TEST_LIB})
# set_property(TARGET msga_drone PROPERTY POSITION_INDEPENDENT_CODE ON)


add_library(msga_dronex SHARED tests/drone.cpp)
target_link_libraries(msga_dronex msga_static ${TEST_LIB})
# set_property(TARGET msga_dronex PROPERTY POSITION_INDEPENDENT_CODE ON)
add_executable(msga_loader tests/loader.cpp)
target_link_libraries(msga_loader msga_static ${TEST_LIB})
# set_property(TARGET msga_loader PROPERTY POSITION_INDEPENDENT_CODE ON)
# target_link_libraries(msga_test_pe/)


# if(WIN32)
    add_executable(msga_io tests/test_io.cpp)
    target_link_libraries(msga_io msga_static)
# endif()