cmake_minimum_required(VERSION 3.0.0)

project(EclipseEngine)

set(BUILD_SHARED_LIBS OFF)

find_package(Vulkan REQUIRED)

include_directories(third_party/glfw/include)
include_directories(src)
include_directories(${VULKAN_INCLUDE_DIR})

add_subdirectory(third_party/glfw)

link_libraries(Vulkan::Vulkan)

if(WIN32)
link_libraries(glfw)
else()
link_libraries(glfw3)
endif()

add_library(Engine STATIC
    src/Engine/Engine.c
    src/Engine/Engine.h
    src/Engine/Core.c
    src/Engine/Core.h
    src/Engine/Error.c
    src/Engine/Error.h
    src/Engine/TypeDefs.h
)

add_executable(Demo
    src/Demo/Main.c
)

target_link_libraries(Demo Engine)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})