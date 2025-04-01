cmake_minimum_required(VERSION 3.20)

project(vulkan_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

find_package(Vulkan REQUIRED)

add_subdirectory(external/glfw)

# Agora vamos adicionar o subdiretório 'src' (onde está o outro CMakeLists).
add_subdirectory(src)

# add_subdirectory(tests) se/quando você tiver testes
