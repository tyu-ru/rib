message(STATUS "Building MaybeRange test")

set(target MaybeRange-tests)
message(STATUS "Building library ${target}")

set(sources MaybeRange-tests.cpp)
add_executable(${target} ${sources})
add_test(MaybeRange "${EXECUTABLE_OUTPUT_PATH}/${target}")
set_tests_properties(MaybeRange PROPERTIES LABELS "Container")