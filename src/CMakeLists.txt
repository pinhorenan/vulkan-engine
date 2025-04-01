add_library(vulkan_engine_lib
        core/Engine.cpp
        core/Device.cpp
        core/Globals.cpp
        core/SwapChain.cpp
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
