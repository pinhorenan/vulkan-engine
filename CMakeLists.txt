cmake_minimum_required(VERSION 3.20)
project(vulkan_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ativa build de Debug por padrão
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Detecta e usa Vulkan e GLFW
find_package(Vulkan REQUIRED)

# Se for compilar GLFW manualmente:
add_subdirectory(external/glfw)

add_subdirectory(src)
