# Add return5 library
add_library(return5 STATIC "")
target_sources(
    return5
    PRIVATE return5.c return5.h)
target_include_directories(
    return5
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(BISON)
find_package(FLEX)
BISON_TARGET(MyshParser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
FLEX_TARGET(MyshScanner lexer.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
ADD_FLEX_BISON_DEPENDENCY(MyshScanner MyshParser)
message(${CMAKE_CURRENT_BINARY_DIR})
message(${BISON_MyshParser_OUTPUTS})
add_executable(Foo
    ${BISON_MyshParser_OUTPUTS}
    ${FLEX_MyshScanner_OUTPUTS}
    ${BISON_MyshParser_OUTPUT_HEADER}
)

target_include_directories(
    Foo
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
 
# Project executable
add_executable(mysh main.c)
target_link_libraries(mysh return5)
