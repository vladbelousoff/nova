cmake_minimum_required(VERSION 3.0)
project(nova)

find_package(OpenGL REQUIRED)

# Compile external dependencies
add_subdirectory(external)

include_directories(
        external/glfw/include/
        external/glm/
        external/glew/include/
        .
)

set(
        ALL_LIBS
        ${OPENGL_LIBRARY}
        glfw
        glew
)

add_definitions(
        -DTW_STATIC
        -DTW_NO_LIB_PRAGMA
        -DTW_NO_DIRECT3D
        -DGLEW_STATIC
        -D_CRT_SECURE_NO_WARNINGS
)

set(
        CMAKE_CXX_STANDARD 17
)

set(
        SOURCES
        main.cpp
        Shader.cpp
)

add_executable(
        ${PROJECT_NAME}
        ${SOURCES}
)

target_link_libraries(
        ${PROJECT_NAME}
        ${ALL_LIBS}
)

file(
        COPY ${CMAKE_CURRENT_SOURCE_DIR}/data
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)