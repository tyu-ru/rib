message(STATUS "Building MaybeAdapter test")

set(target MaybeAdapter-tests)
message(STATUS "Building library ${target}")

set(sources MaybeAdapter-tests.cpp)
add_executable(${target} ${sources})
add_test(MaybeAdapter "${EXECUTABLE_OUTPUT_PATH}/${target}")
set_tests_properties(MaybeAdapter PROPERTIES LABELS "Monad")
