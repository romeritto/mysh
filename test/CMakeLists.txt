project (tests C)

find_package(cmocka REQUIRED)

add_executable(return5_test return5_test.c)
target_include_directories(return5_test PRIVATE ${CMOCKA_INCLUDE_DIR})
target_link_libraries(return5_test return5 ${CMOCKA_LIBRARY})
add_test(return5_test return5_test)
