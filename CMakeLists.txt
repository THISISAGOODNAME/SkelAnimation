cmake_minimum_required(VERSION 3.17)
project(SkelAnimation)

set(CMAKE_CXX_STANDARD 14)

# GLFW3
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(3rd_party/glfw)

# glad
set(GLAD_FILES 3rd_party/glad/src/gl.c)

# INCLUDE
include_directories(
        3rd_party/glad/include
        3rd_party/glfw/include
        3rd_party/Nuklear
        3rd_party/stb
        src
        )

# LIBs
set(COMMON_LIBS glfw ${GLFW_LIBRARIES} ${GLAD_LIBRARIES})

# Assets & Shaders
add_custom_target(copy-runtime-files ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Assets ${CMAKE_BINARY_DIR}/Assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Shaders ${CMAKE_BINARY_DIR}/Shaders
        )

# Source
## Math
file(GLOB MATH_SRC src/Math/*.cpp)

## RHI
file(GLOB RHI_SRC src/RHI/*.cpp src/RHI/*/*/*.cpp)

## Misc
file(GLOB MISC_SRC src/Misc/*.cpp)

add_executable(SkelAnimation src/main.cpp ${GLAD_FILES} ${MATH_SRC} ${RHI_SRC} ${MISC_SRC})
target_link_libraries(SkelAnimation ${COMMON_LIBS})
add_dependencies(SkelAnimation copy-runtime-files)
