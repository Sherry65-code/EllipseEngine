cmake_minimum_required(VERSION 3.5)

project(EclipseEngine)

set(BUILD_SHARED_LIBS OFF)

find_package(Vulkan REQUIRED)

include_directories(third_party/glfw/include)
include_directories(src)
include_directories(${VULKAN_INCLUDE_DIR})

add_subdirectory(third_party/glfw)

link_libraries(Vulkan::Vulkan)

link_libraries(glfw)

file(GLOB_RECURSE SRC_FILES src/Engine/*)

add_library(Engine STATIC
    ${SRC_FILES}
)

add_executable(Editor
    src/Editor/Main.c
)

target_link_libraries(Editor Engine)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})