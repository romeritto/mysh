find_package(BISON)
find_package(FLEX)
BISON_TARGET(MyshBison parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
FLEX_TARGET(MyshFlex lexer.l  ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
ADD_FLEX_BISON_DEPENDENCY(MyshFlex MyshBison)
message(${CMAKE_CURRENT_BINARY_DIR})

add_library(MyshParser STATIC "")
target_sources(
    MyshParser 
    	PUBLIC
	${BISON_MyshBison_OUTPUTS}
	PRIVATE 
	${FLEX_MyshFlex_OUTPUTS}
	cmd.c
	utils.c)
target_include_directories(
    MyshParser 
    PRIVATE 
    	${CMAKE_CURRENT_SOURCE_DIR})

# Project executable
add_executable(mysh main.c execute.c utils.c)
target_link_libraries(mysh MyshParser)
target_link_libraries(mysh readline)
target_include_directories(
	mysh
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})
