message(STATUS "Building Optional test")

set(target Optional-tests)
message(STATUS "Building library ${target}")

set(sources Optional-tests.cpp)
add_executable(${target} ${sources})
add_test(Optional "${EXECUTABLE_OUTPUT_PATH}/${target}")
set_tests_properties(Optional PROPERTIES LABELS "Container")