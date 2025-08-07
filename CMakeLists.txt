cmake_minimum_required(VERSION 4.0)
project(BaseRunner VERSION 0.1.0 LANGUAGES CXX)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/dist" CACHE PATH "Install path prefix." FORCE)
endif()

# Force UTF-8 on MSVC
if(MSVC)
    add_compile_options("/utf-8")
endif()

# === version.h ===

set(VERSION_TEMPLATE "${CMAKE_SOURCE_DIR}/src/version.h.in")
set(VERSION_HEADER "${CMAKE_BINARY_DIR}/generated/version.h")
set(GENERATE_SCRIPT "${CMAKE_SOURCE_DIR}/tools/generate_version.py")

add_custom_command(
        OUTPUT "${VERSION_HEADER}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/generated"
        COMMAND ${CMAKE_COMMAND} -E env PYTHONIOENCODING=utf-8
        ${Python3_EXECUTABLE} "${GENERATE_SCRIPT}"
        "${VERSION_TEMPLATE}"
        "${VERSION_HEADER}"
        "${PROJECT_NAME}"
        "${PROJECT_VERSION}"
        DEPENDS "${VERSION_TEMPLATE}" "${GENERATE_SCRIPT}"
        COMMENT "Generating version.h"
)

add_custom_target(GenerateVersionHeader ALL DEPENDS "${VERSION_HEADER}")

include_directories("${CMAKE_BINARY_DIR}/generated")

# === SDL2 ===
set(SDL2_DIR vendor/SDL2-2.32.8)
include_directories(${SDL2_DIR}/include)
link_directories(${SDL2_DIR}/lib/x64)

# === FreeType ===
set(FREETYPE_DIR vendor/freetype-windows-binaries-2.13.3)
include_directories(${FREETYPE_DIR}/include)
link_directories("${FREETYPE_DIR}/release static/vs2015-2022/win64")

# === ImGui ===
set(IMGUI_DIR vendor/imgui-1.92.1)
file(GLOB IMGUI_SOURCES
        ${IMGUI_DIR}/*.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl2.cpp
        ${IMGUI_DIR}/misc/freetype/imgui_freetype.cpp
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

include_directories(
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${IMGUI_DIR}/misc/freetype
        ${IMGUI_DIR}/misc/cpp
)

# === plog ===
set(PLOG_DIR vendor/plog-1.1.10)
include_directories(
        ${PLOG_DIR}/include
)

# === Project build ===
file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp)
file(GLOB_RECURSE PROJECT_HEADERS src/*.h src/*.hpp)

include_directories(src)

# Add resource file for icon (Windows only)
if (WIN32)
    enable_language(RC)
    set(WIN_ICON resources/resources.rc)
endif()

add_executable(BaseRunner
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${IMGUI_SOURCES}
        ${WIN_ICON}
)

add_dependencies(BaseRunner GenerateVersionHeader)

if(WIN32)
    target_link_libraries(BaseRunner
            SDL2 SDL2main
            opengl32
            freetype
    )

    # Copy SDL2.dll after build
    add_custom_command(TARGET BaseRunner POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/${SDL2_DIR}/lib/x64/SDL2.dll
            $<TARGET_FILE_DIR:BaseRunner>)

    # === Assets (copy to binary dir) ===
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
else()
    message(FATAL_ERROR "Only Windows with prebuilt SDL2 is supported in this setup")
endif()

# === Project install ===

set(DESTINATION_PATH "${PROJECT_NAME}-${PROJECT_VERSION}")

install(TARGETS BaseRunner DESTINATION ${DESTINATION_PATH}/bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/ DESTINATION ${DESTINATION_PATH}/bin)
install(FILES ${CMAKE_SOURCE_DIR}/${SDL2_DIR}/lib/x64/SDL2.dll DESTINATION ${DESTINATION_PATH}/bin)
