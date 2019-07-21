message(STATUS "Building splitString test")

set(target splitString-tests)
message(STATUS "Building library ${target}")

set(sources splitString-tests.cpp)
add_executable(${target} ${sources})
add_test(splitString "${EXECUTABLE_OUTPUT_PATH}/${target}")
set_tests_properties(splitString PROPERTIES LABELS "Utils")
