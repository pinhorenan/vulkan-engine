# Para evitar conflitos, podemos manter a política de versionamento do CMake acima de 3.20
cmake_minimum_required(VERSION 3.20)

# Cria uma biblioteca estática para o motor
add_library(vulkan_engine_lib
        core/Engine.cpp
        # gfx/*.cpp
        # sim/*.cpp
        # etc.
)

target_include_directories(vulkan_engine_lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(vulkan_engine_lib
        PUBLIC
        Vulkan::Vulkan
)

add_executable(vulkan_engine_app
        core/main.cpp
)

target_link_libraries(vulkan_engine_app
        PRIVATE
        vulkan_engine_lib
        glfw
)

if(UNIX AND NOT APPLE)
    target_link_libraries(vulkan_engine_app PRIVATE dl pthread)
endif()
