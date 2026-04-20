project(BoyFramework VERSION 1.35 LANGUAGES CXX)

set(BOYLIB_INCLUDES
${CMAKE_SOURCE_DIR}/libs/BoyFramework/
${CMAKE_SOURCE_DIR}/libs/BoyFramework/BoyLib/)

file(GLOB_RECURSE BOYLIB_SOURCES "BoyLib/*.cpp")

set(BOY_INCLUDES
${CMAKE_SOURCE_DIR}/libs/BoyFramework/Boy/)

file(GLOB_RECURSE BOY_SOURCES "Boy/*.cpp")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${BOYLIB_INCLUDES} ${BOY_INCLUDES} external)
add_library(BoyFramework STATIC ${BOYLIB_SOURCES} ${BOY_SOURCES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(BoyFramework PRIVATE $<$<CONFIG:Debug>:_DEBUG>)
endif()

if (WIN32)
    target_link_libraries(BoyFramework PRIVATE  SDL2::SDL2-static SDL2_image::SDL2_image-static SDL2_mixer::SDL2_mixer-static OpenGL::GL TinyXML)
else()
    target_link_libraries(BoyFramework PRIVATE  SDL2 SDL2_image SDL2_mixer OpenGL::GL TinyXML)
endif()

add_subdirectory(external)
