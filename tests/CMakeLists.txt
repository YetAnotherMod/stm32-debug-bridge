cmake_minimum_required(VERSION 3.13)

project (tests CXX)

include_directories(../include)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)
add_compile_options(-O3 -std=c++2a --coverage -gdwarf-2)
add_link_options(--coverage)
link_libraries(pthread)

add_executable(fifo fifo.cpp)
add_executable(fifo2 fifo2.cpp)
add_executable(fifo3 fifo3.cpp)
add_executable(static_map static_map.cpp)
add_executable(shell shell.cpp)

include(CTest)

add_test(fifo fifo)
add_test(fifo2 fifo2)
add_test(fifo3 fifo3)
add_test(static_map static_map)
add_test(shell shell)

if(NOT REPORT_PATH)
    set(REPORT_PATH ${PROJECT_BINARY_DIR})
endif(NOT REPORT_PATH)

add_custom_target(coverage
    COMMAND find "'('" -name *.cpp.gcno "')'" -exec gcov "{}" "+" > /dev/null
    COMMAND lcov -q -o tests.info -t "tests" -c -d . --rc lcov_branch_coverage=1 --exclude '/usr/include/*'
    COMMAND genhtml -q -o ${REPORT_PATH}/coverage_report tests.info --rc lcov_branch_coverage=1
)
