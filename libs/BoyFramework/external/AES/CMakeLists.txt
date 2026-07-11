project(AES VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE AES_SOURCES "*.cpp")
add_library(AES STATIC ${AES_SOURCES})

target_include_directories(AES PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)