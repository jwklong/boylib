cmake_minimum_required(VERSION 3.21...4.2)
set(CMAKE_CXX_STANDARD 23)

project(Asteroids VERSION 1.0 LANGUAGES CXX)

add_subdirectory(libs)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_custom_target(ADD_RESOURCES ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
    ${PROJECT_SOURCE_DIR}/res
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res
    COMMENT "Adding resources to build...")

add_executable(Asteroids ${SOURCES})

add_dependencies(Asteroids ADD_RESOURCES)

target_include_directories(Asteroids PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/BoyFramework/
    ${CMAKE_SOURCE_DIR}/libs/BoyFramework/Boy/
    ${CMAKE_SOURCE_DIR}/libs/BoyFramework/BoyLib/
    ${CMAKE_SOURCE_DIR}/libs/BoyFramework/external/)

target_link_libraries(Asteroids PRIVATE BoyFramework)

if (WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(Asteroids PRIVATE -static)
    else()
        target_link_options(Asteroids PRIVATE -static -mwindows)
    endif()

    set(REQUIRED_DLLS
        libaom.dll
        libavif-16.dll
        libbrotlicommon.dll
        libbrotlidec.dll
        libbrotlienc.dll
        libdav1d-7.dll
        libdeflate.dll
        libFLAC.dll
        libgcc_s_seh-1.dll
        libhwy.dll
        libjbig-0.dll
        libjpeg-8.dll
        libjxl.dll
        libjxl_cms.dll
        liblcms2-2.dll
        libLerc.dll
        liblzma-5.dll
        libmpg123-0.dll
        libogg-0.dll
        libopus-0.dll
        libopusfile-0.dll
        libpng16-16.dll
        librav1e.dll
        libsharpyuv-0.dll
        libstdc++-6.dll
        libSvtAv1Enc-4.dll
        libtiff-6.dll
        libvorbis-0.dll
        libvorbisfile-3.dll
        libwavpack-1.dll
        libwebp-7.dll
        libwebpdemux-2.dll
        libwinpthread-1.dll
        libxmp.dll
        libyuv.dll
        libzstd.dll
        zlib1.dll
    )

    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    set(DLL_PATHS "")
    foreach(DLL_NAME ${REQUIRED_DLLS})
        list(APPEND DLL_PATHS "${MINGW_BIN_DIR}/${DLL_NAME}")
    endforeach()

    add_custom_command(TARGET Asteroids POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${DLL_PATHS}
            "$<TARGET_FILE_DIR:Asteroids>"
        COMMENT "Copying runtime DLLs..."
    )
endif()