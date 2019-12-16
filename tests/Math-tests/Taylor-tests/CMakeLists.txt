message(STATUS "Building Taylor test")

set(target Taylor-tests)
message(STATUS "Building library ${target}")

set(sources Taylor-tests.cpp)
add_executable(${target} ${sources})
add_test(Taylor "${EXECUTABLE_OUTPUT_PATH}/${target}")
set_tests_properties(Taylor PROPERTIES LABELS "Math")
