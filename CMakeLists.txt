cmake_minimum_required(VERSION 3.5)

project(EclipseEngine)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

include_directories(third_party/glfw/include)
include_directories(third_party/fmt/include)
include_directories(${VULKAN_INCLUDE_DIR})

add_subdirectory(third_party/glfw)
add_subdirectory(third_party/fmt)

link_libraries(Vulkan::Vulkan)
link_libraries(fmt::fmt)
link_libraries(glfw)

file(GLOB_RECURSE SRC_FILES src/*)

add_executable(EclipseEngine
    ${SRC_FILES}
)

file(GLOB_RECURSE SHADERS_FILES shaders/*)

add_custom_target(Shaders SOURCES ${SHADERS_FILES})

foreach(Shader ${SHADERS_FILES})
    get_filename_component(Shader_name ${Shader} NAME_WE)
    add_custom_command(
        TARGET Shaders
        PRE_BUILD
        COMMAND ${Vulkan_INCLUDE_DIRS}/../bin/glslc ${Shader} -o ${CMAKE_BINARY_DIR}/${Shader_name}.spv
        COMMENT "Compiling shader ${Shader_name}"
    )
endforeach()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
