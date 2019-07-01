message(STATUS "Building UniqPtrVector test")

set(target UniqPtrVector-tests)
message(STATUS "Building library ${target}")

set(sources UniqPtrVector-tests.cpp)
add_executable(${target} ${sources})
add_test(UniqPtrVector "${EXECUTABLE_OUTPUT_PATH}/${target}")
set_tests_properties(UniqPtrVector PROPERTIES LABELS "Container")