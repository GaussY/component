cmake_minimum_required(VERSION 3.20)

project(Translator)

set(HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/TranslatorModule.h"
)

set(SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TranslatorModule.cpp"
)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/Modules/Interface/include"
)

target_link_libraries(${PROJECT_NAME}
    ThreadTask
)

# DEBUG版本目标文件名后缀
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
