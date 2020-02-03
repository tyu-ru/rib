message(STATUS "Building Expected test")

set(target Expected-tests)
message(STATUS "Building library ${target}")

set(sources Expected-tests.cpp)
add_executable(${target} ${sources})
add_test(Expected "${EXECUTABLE_OUTPUT_PATH}/${target}")
set_tests_properties(Expected PROPERTIES LABELS "Monad")
