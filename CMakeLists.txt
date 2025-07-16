cmake_minimum_required(VERSION 3.20)

project(freecell C)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_C_STANDARD 23)
    set(FETCHCONTENT_QUIET FALSE)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

Include(FetchContent)

# Setup glfw
function(glfw_build)
    set(GLFW_BUILD_EXAMPLES  OFF CACHE INTERNAL "Build the GLFW example programs")
    set(GLFW_BUILD_TESTS     OFF CACHE INTERNAL "Build the GLFW test programs")
    set(GLFW_BUILD_DOCS      OFF CACHE INTERNAL "Build the GLFW documentation")
    set(GLFW_INSTALL         OFF CACHE INTERNAL "Generate installation target")
    set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE INTERNAL "Use MSVC Runtime")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" CACHE STRING "" FORCE)
    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw
            GIT_TAG        3.4
            GIT_SHALLOW 1
            GIT_PROGRESS 1
    )

    FetchContent_MakeAvailable(glfw)
endfunction()
glfw_build()


# Setup glad
function(glad_build)
    set(GLAD_PROFILE    "core"  CACHE INTERNAL "")
    set(GLAD_API        "gl="   CACHE INTERNAL "")
    set(GLAD_GENERATOR  "c"     CACHE INTERNAL "")
    FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad
        GIT_TAG        v0.1.34
        GIT_SHALLOW 1
        GIT_PROGRESS 1
    )

    FetchContent_MakeAvailable(glad)
endfunction()
glad_build()

# Setup stb
function(stb_build)
    FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb
        GIT_TAG        master
        GIT_SHALLOW 1
        GIT_PROGRESS 1
    )

    FetchContent_MakeAvailable(stb)
    add_library(stb INTERFACE)
    add_library(stb::stb ALIAS stb)
    target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endfunction()
stb_build()

# Setup cglm
function(cglm_build)
    FetchContent_Declare(
        cglm
        GIT_REPOSITORY https://github.com/recp/cglm
        GIT_TAG        v0.9.6
        GIT_SHALLOW 1
        GIT_PROGRESS 1
    )

    set(CGLM_SHARED OFF CACHE INTERNAL "Shared build")
    set(CGLM_STATIC ON CACHE INTERNAL "Static build")
    set(CGLM_USE_C99 ON CACHE INTERNAL "") # C11 
    FetchContent_MakeAvailable(cglm)
endfunction()
cglm_build()


# Setup miniaudio
function(miniaudio_build)
    FetchContent_Declare(
      miniaudio
      GIT_REPOSITORY https://github.com/mackron/miniaudio.git
      GIT_TAG        0.11.22
      GIT_SHALLOW 1
      GIT_PROGRESS 1
    )
    FetchContent_MakeAvailable(miniaudio)
    target_compile_definitions(miniaudio PRIVATE MA_NO_ENCODING MA_NO_FLAC MA_NO_MP3 MA_NO_GENERATION)
endfunction()
miniaudio_build()

find_package(OpenGL REQUIRED)

# Main exe
add_executable(
    freecell 
    src/main.c

    src/game/assets.c
    src/game/card_sprites.c
    src/game/constants.c
    src/game/controller.c
    src/game/debug.c
    src/game/input.c
    src/game/render_system.c
    src/game/ui_layout.c
    src/game/ui_state.c
    src/game/world.c

    src/core/freecell.c
    src/core/game.c
    src/core/vector.c
    src/core/log.c
    src/core/stb_image.c

    src/rendering/mesh.c
    src/rendering/image.c
    src/rendering/renderer.c
    src/rendering/shader.c
    src/rendering/texture.c

    src/platform/window.c

    src/utils.c
)
set_property(TARGET freecell PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
target_link_options(freecell PRIVATE -static)
target_link_libraries(freecell OpenGL::GL glad glfw cglm stb::stb miniaudio User32.lib)
target_include_directories(freecell PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Tests
add_executable(
    tests 
    src/test/test.c 
    src/core/freecell.c
)

target_link_libraries(tests User32.lib)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src )

target_link_options(tests PRIVATE)
