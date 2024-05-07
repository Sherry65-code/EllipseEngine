cmake_minimum_required(VERSION 3.5)

project(EclipseEngine)

set(BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

include_directories(third_party/glfw/include)
include_directories(third_party/volk)
include_directories(src)
include_directories(${VULKAN_INCLUDE_DIR})

add_subdirectory(third_party/glfw)
add_subdirectory(third_party/volk)

link_libraries(Vulkan::Vulkan)
link_libraries(volk::volk)
link_libraries(glfw)

file(GLOB_RECURSE SRC_FILES src/*)

add_executable(EclipseEngine
    ${SRC_FILES}
)

file(GLOB_RECURSE SHADERS_FILES shaders/*)

add_custom_target(Shaders SOURCES ${SHADERS_FILES})

if (WIN32)
foreach(Shader ${SHADERS_FILES})
    get_filename_component(ShaderName ${Shader} NAME_WE)  # Extract shader file name without extension
    add_custom_command(
        TARGET Shaders
        PRE_BUILD
        COMMAND ${Vulkan_INCLUDE_DIRS}/../Bin/glslc ${Shader} -o ${CMAKE_BINARY_DIR}/${ShaderName}.spv
        COMMENT "Compiling shader ${Shader}"
    )
endforeach()
else()
foreach(Shader ${SHADERS_FILES})
    get_filename_component(ShaderName ${Shader} NAME_WE)  # Extract shader file name without extension
    add_custom_command(
        TARGET Shaders
        PRE_BUILD
        COMMAND ${CMAKE_SOURCE_DIR}/linux/glslc ${Shader} -o ${CMAKE_BINARY_DIR}/${ShaderName}.spv
        COMMENT "Compiling shader ${Shader}"
    )
endforeach()
endif()

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
