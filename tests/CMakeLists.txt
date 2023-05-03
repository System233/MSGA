


set(TEST_LINK_LIB msga_static)

if(UNIX)
set(TEST_LINK_LIB ${TEST_LINK_LIB} dl)
endif()

link_libraries(${TEST_LINK_LIB})



add_executable(target target.cpp)

add_library(target_shared SHARED target.cpp)

add_executable(loader loader.cpp)
set_target_properties(loader PROPERTIES DEPENDS target_shared)

add_executable(test_io_rt test_io_rt.cpp)


add_executable(test_io_pe test_io_pe.cpp)
set_target_properties(test_io_pe PROPERTIES DEPENDS target)


add_executable(test_io_elf test_io_elf.cpp)
set_target_properties(test_io_pe PROPERTIES DEPENDS target)

set(TEST_HOOK_FROM test_add)
set(TEST_HOOK_TO hooked_add)
set(TEST_HOOK_ORIG pfunc_add)



add_test(NAME Test::IO::Runtime COMMAND test_io_rt)


function(make_test NAME X Y)
set(TEST_ADD_X ${X})
set(TEST_ADD_Y ${Y})
math(EXPR TEST_ADD_RESULT "${TEST_ADD_X}+${TEST_ADD_Y}")
math(EXPR TEST_ADD_HOOKED_RESULT "${TEST_ADD_X}*2+${TEST_ADD_Y}+1") 
add_test(NAME ${NAME}::Target::SimpleAdd
    COMMAND $<TARGET_FILE:target> ${TEST_ADD_RESULT} ${TEST_ADD_X} ${TEST_ADD_Y} )
set_tests_properties(${NAME}::Target::SimpleAdd PROPERTIES 
        PASS_REGULAR_EXPRESSION "test_add"
        FAIL_REGULAR_EXPRESSION "hooked_add")
add_test(NAME ${NAME}::Target::SimpleAdd_SF
        COMMAND $<TARGET_FILE:target> ${TEST_ADD_HOOKED_RESULT} ${TEST_ADD_X} ${TEST_ADD_Y} )
set_tests_properties(${NAME}::Target::SimpleAdd_SF PROPERTIES WILL_FAIL TRUE)
if(WIN32)
    set(TEST_OUTPUT output_${NAME}.exe)
    add_test(
        NAME ${NAME}::IO::PE::EXE::HOOK
        COMMAND test_io_pe $<TARGET_FILE:target>
                        ${TEST_OUTPUT} 
                        ${TEST_HOOK_FROM} 
                        ${TEST_HOOK_TO} 
                        ${TEST_HOOK_ORIG})
    add_test(NAME ${NAME}::IO::PE::EXE::OUTPUT_SF COMMAND ${TEST_OUTPUT} ${TEST_ADD_RESULT} ${TEST_ADD_X} ${TEST_ADD_Y})
    set_tests_properties(${NAME}::IO::PE::EXE::OUTPUT_SF PROPERTIES WILL_FAIL TRUE)

    add_test(NAME ${NAME}::IO::PE::EXE::OUTPUT COMMAND ${TEST_OUTPUT} ${TEST_ADD_HOOKED_RESULT} ${TEST_ADD_X} ${TEST_ADD_Y} )
    set_tests_properties(${NAME}::IO::PE::EXE::OUTPUT PROPERTIES  PASS_REGULAR_EXPRESSION "hooked_add")

    add_test(NAME ${NAME}::IO::PE::EXE::CLEAN COMMAND rm ${TEST_OUTPUT})
    set_tests_properties(${NAME}::IO::PE::EXE::HOOK PROPERTIES FIXTURES_SETUP ${NAME}::IO::PE::EXE)
    set_tests_properties(${NAME}::IO::PE::EXE::OUTPUT ${NAME}::IO::PE::EXE::OUTPUT_SF PROPERTIES FIXTURES_REQUIRED ${NAME}::IO::PE::EXE)
    set_tests_properties(${NAME}::IO::PE::EXE::CLEAN PROPERTIES FIXTURES_CLEANUP ${NAME}::IO::PE::EXE)

    
    set(TEST_LIB_OUTPUT liboutput_${NAME}.dll)
    add_test(
        NAME ${NAME}::IO::PE::DLL::HOOK
        COMMAND test_io_pe $<TARGET_FILE:target_shared> 
                        ${TEST_LIB_OUTPUT} 
                        ${TEST_HOOK_FROM} 
                        ${TEST_HOOK_TO} 
                        ${TEST_HOOK_ORIG})
    add_test(NAME ${NAME}::IO::PE::DLL::OUTPUT_SF COMMAND loader ./${TEST_LIB_OUTPUT} ${TEST_ADD_RESULT} ${TEST_ADD_X} ${TEST_ADD_Y} )
    set_tests_properties(${NAME}::IO::PE::DLL::OUTPUT_SF PROPERTIES WILL_FAIL TRUE)
    
    add_test(NAME ${NAME}::IO::PE::DLL::OUTPUT COMMAND loader ./${TEST_LIB_OUTPUT} ${TEST_ADD_HOOKED_RESULT} ${TEST_ADD_X} ${TEST_ADD_Y} )
    set_tests_properties(${NAME}::IO::PE::DLL::OUTPUT PROPERTIES PASS_REGULAR_EXPRESSION "hooked_add")
    add_test(NAME ${NAME}::IO::PE::DLL::CLEAN COMMAND rm ${TEST_LIB_OUTPUT})
    set_tests_properties(${NAME}::IO::PE::DLL::HOOK PROPERTIES FIXTURES_SETUP ${NAME}::IO::PE::DLL)
    set_tests_properties(${NAME}::IO::PE::DLL::OUTPUT ${NAME}::IO::PE::DLL::OUTPUT_SF 
                        PROPERTIES FIXTURES_REQUIRED ${NAME}::IO::PE::DLL)
    set_tests_properties(${NAME}::IO::PE::DLL::CLEAN PROPERTIES FIXTURES_CLEANUP ${NAME}::IO::PE::DLL)

endif()
if(UNIX)
    set(TEST_OUTPUT output_${NAME})
    add_test(NAME ${NAME}::IO::ELF::HOOK 
            COMMAND test_io_elf 
                $<TARGET_FILE:target>
                ${TEST_OUTPUT} 
                ${TEST_HOOK_FROM} 
                ${TEST_HOOK_TO} 
                ${TEST_HOOK_ORIG})
        
    add_test(NAME ${NAME}::IO::ELF::OUTPUT_SF COMMAND ${TEST_OUTPUT} ${TEST_ADD_RESULT} ${TEST_ADD_X} ${TEST_ADD_Y})
    set_tests_properties(${NAME}::IO::ELF::OUTPUT_SF PROPERTIES WILL_FAIL TRUE)

    add_test(NAME ${NAME}::IO::ELF::OUTPUT COMMAND ${TEST_OUTPUT} ${TEST_ADD_HOOKED_RESULT} ${TEST_ADD_X} ${TEST_ADD_Y})
    set_tests_properties(${NAME}::IO::ELF::OUTPUT PROPERTIES PASS_REGULAR_EXPRESSION "hooked_add")

    add_test(NAME ${NAME}::IO::ELF::CLEAN COMMAND rm ${TEST_OUTPUT})
    set_tests_properties(${NAME}::IO::ELF::HOOK PROPERTIES FIXTURES_SETUP ${NAME}::IO::ELF)
    set_tests_properties(${NAME}::IO::ELF::CLEAN  PROPERTIES FIXTURES_CLEANUP ${NAME}::IO::ELF)
    set_tests_properties(${NAME}::IO::ELF::OUTPUT ${NAME}::IO::ELF::OUTPUT_SF PROPERTIES FIXTURES_REQUIRED ${NAME}::IO::ELF)

    set(TEST_LIB_OUTPUT liboutput_${NAME}.so)
    add_test(NAME ${NAME}::IO::ELF::SO::HOOK
            COMMAND test_io_elf 
                    $<TARGET_FILE:target_shared> 
                    ${TEST_LIB_OUTPUT} 
                    ${TEST_HOOK_FROM} 
                    ${TEST_HOOK_TO} 
                    ${TEST_HOOK_ORIG})
    add_test(NAME ${NAME}::IO::ELF::SO::OUTPUT_SF COMMAND loader ./${TEST_LIB_OUTPUT} ${TEST_ADD_RESULT} ${TEST_ADD_X} ${TEST_ADD_Y} )
    set_tests_properties(${NAME}::IO::ELF::SO::OUTPUT_SF PROPERTIES WILL_FAIL TRUE)
    
    add_test(NAME ${NAME}::IO::ELF::SO::OUTPUT COMMAND loader ./${TEST_LIB_OUTPUT} ${TEST_ADD_HOOKED_RESULT} ${TEST_ADD_X} ${TEST_ADD_Y} )
    set_tests_properties(${NAME}::IO::ELF::SO::OUTPUT PROPERTIES PASS_REGULAR_EXPRESSION "hooked_add")

    add_test(NAME ${NAME}::IO::ELF::SO::CLEAN COMMAND rm ${TEST_LIB_OUTPUT})

    set_tests_properties(${NAME}::IO::ELF::SO::HOOK PROPERTIES FIXTURES_SETUP ${NAME}::IO::ELF::SO)
    set_tests_properties(${NAME}::IO::ELF::SO::OUTPUT ${NAME}::IO::ELF::SO::OUTPUT_SF 
                        PROPERTIES FIXTURES_REQUIRED ${NAME}::IO::ELF::SO)
    set_tests_properties(${NAME}::IO::ELF::SO::CLEAN PROPERTIES FIXTURES_CLEANUP ${NAME}::IO::ELF::SO)

endif()

endfunction(make_test NAME X Y)

make_test(Case1 123456 789123)
make_test(Case2 987654 654321)



if(WIN32)
add_executable(inject win32/inject.cpp)
add_library(nokill SHARED win32/nokill.cpp)

add_executable(blocker win32/noinject.cpp)
target_link_libraries(blocker ntdll)
endif()