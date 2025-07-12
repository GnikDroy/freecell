cmake_minimum_required(VERSION 3.20)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
            STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
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

find_package(OpenGL REQUIRED)

project(freecell C)

# Main exe
add_executable(
    freecell 
    WIN32 
    src/main.c

    src/assets.c
    src/card_sprites.c
    src/constants.c
    src/debug.c
    src/game.c
    src/image.c
    src/renderer.c
    src/utils.c
    src/vector.c
    src/window.c
    src/world.c
    src/log.c
    src/shader.c
    src/render_system.c

    src/stb_image.c
)
set_property(TARGET freecell PROPERTY MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
target_link_options(freecell PRIVATE -static)
target_link_libraries(freecell OpenGL::GL stb::stb glfw glad User32.lib)
target_include_directories(freecell PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Tests
add_executable(
    tests 
    src/test.c 

    src/vector.c 
    src/world.c
    src/assets.c
    src/constants.c
    src/image.c
    src/card_sprites.c
    src/utils.c

    src/stb_image.c
)

target_link_libraries(tests stb::stb User32.lib)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src )

target_link_options(tests PRIVATE)
