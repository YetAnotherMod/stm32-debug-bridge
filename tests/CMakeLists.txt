cmake_minimum_required(VERSION 3.13)

project (tests CXX)

include_directories(../include)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)
add_compile_options(-O0 -std=c++2a --coverage)
add_link_options(--coverage)
link_libraries(pthread)

add_executable(fifo fifo.cpp)
add_executable(fifo2 fifo2.cpp)

include(CTest)

add_test(fifo fifo)
add_test(fifo2 fifo2)

target_compile_options(fifo2 PRIVATE -O0)

if(NOT REPORT_PATH)
    set(REPORT_PATH ${PROJECT_BINARY_DIR})
endif(NOT REPORT_PATH)

add_custom_target(coverage
    COMMAND find "'('" -name *.cpp.gcno -or  -name *.cpp.gcda "')'" -exec gcov "{}" "+"
    COMMAND lcov -o tests-raw.info -t "tests" -c -d . --rc lcov_branch_coverage=1
    COMMAND lcov -o tests.info -r tests-raw.info '/usr/include/*' --rc lcov_branch_coverage=1
    COMMAND genhtml -o ${REPORT_PATH}/coverage_report tests.info --rc lcov_branch_coverage=1
)
