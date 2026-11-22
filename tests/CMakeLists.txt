add_executable(test_core test_core.cpp)
target_link_libraries(test_core PRIVATE examseat_core)
add_test(NAME core COMMAND test_core)

add_executable(test_contracts test_contracts.cpp)
target_link_libraries(test_contracts PRIVATE examseat_core)
add_test(NAME contracts COMMAND test_contracts)
