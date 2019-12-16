message(STATUS "Building NumericAnalysis test")

set(target NumericAnalysis-tests)
message(STATUS "Building library ${target}")

set(sources NumericAnalysis-tests.cpp)
add_executable(${target} ${sources})
add_test(NumericAnalysis "${EXECUTABLE_OUTPUT_PATH}/${target}")
set_tests_properties(NumericAnalysis PROPERTIES LABELS "Math")
