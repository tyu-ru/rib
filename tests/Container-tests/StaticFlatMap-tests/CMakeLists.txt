message(STATUS "Building StaticFlatMap test")

set(target StaticFlatMap-tests)
message(STATUS "Building library ${target}")

set(sources StaticFlatMap-tests.cpp)
add_executable(${target} ${sources})
add_test(StaticFlatMap "${EXECUTABLE_OUTPUT_PATH}/${target}")
set_tests_properties(StaticFlatMap PROPERTIES LABELS "Container")