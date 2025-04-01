cmake_minimum_required(VERSION 3.20)

# Nome do projeto
project(VulkanEngineProject LANGUAGES CXX)

# Define a versão do C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Opção para compilar com configurações de debug por padrão
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Para encontrar o Vulkan SDK no sistema (é preciso que o SDK esteja instalado)
find_package(Vulkan REQUIRED)

# Para localizar o GLFW. Caso você não utilize "find_package",
# pode compilar GLFW manualmente colocando-o em external/glfw
# e incluindo o add_subdirectory(external/glfw) etc.
# Com pacotes instalados no sistema:
find_package(GLFW3 3.3 REQUIRED)
# Se não funcionar, comente a linha acima e faça a compilação manual.

# Adiciona subdiretório que contém o CMakeLists do src
add_subdirectory(src)

# Se quiser adicionar testes futuramente
# add_subdirectory(tests)
