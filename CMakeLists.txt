cmake_minimum_required(VERSION 3.17)
project(SkelAnimation)

set(CMAKE_CXX_STANDARD 14)

# GLFW3
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(3rd_party/glfw)

# glad.c
set(GLAD_FILES 3rd_party/glad/src/glad.c)

# INCLUDE
include_directories(
        3rd_party/glad/include
        3rd_party/glfw/include
        )

# LIBs
set(COMMON_LIBS glfw ${GLFW_LIBRARIES} ${GLAD_LIBRARIES})

add_executable(SkelAnimation main.cpp ${GLAD_FILES})
target_link_libraries(SkelAnimation ${COMMON_LIBS})