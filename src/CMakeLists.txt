# Add return5 library
add_library(return5 STATIC "")
target_sources(
    return5
    PRIVATE return5.c return5.h)
target_include_directories(
    return5
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Project executable
add_executable(mysh main.c)
target_link_libraries(mysh return5)
