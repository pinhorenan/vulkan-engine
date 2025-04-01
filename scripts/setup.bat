@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul

echo ==================================================
echo   🚀 Vulkan Engine - Setup & Build (Windows)
echo ==================================================

:: 1. Verificar Vulkan SDK
echo.
echo [1/6] Verificando Vulkan SDK...
if not defined VULKAN_SDK (
    echo ❌ Vulkan SDK não está configurado!
    echo 👉 Por favor, instale o Vulkan SDK e abra este script em um terminal com as variáveis de ambiente configuradas.
    exit /b 1
) else (
    echo ✅ Vulkan SDK detectado: %VULKAN_SDK%
)

:: 2. Criar estrutura de pastas
echo.
echo [2/6] Criando estrutura de diretórios...
if not exist build (
    mkdir build
    echo ✔️ build/
) else (
    echo build/ já existe
)

if not exist external (
    mkdir external
    echo ✔️ external/
)

if not exist assets (
    mkdir assets
    mkdir assets\shaders
    echo ✔️ assets/ (e subpasta shaders/)
)

:: 3. Clonar GLFW
echo.
echo [3/6] Verificando GLFW...
if not exist external\glfw (
    echo ⏳ Clonando GLFW para external/glfw...
    git clone --depth=1 https://github.com/glfw/glfw external/glfw
    if errorlevel 1 (
        echo ❌ Falha ao clonar GLFW!
        exit /b 1
    )
    echo ✅ GLFW clonado com sucesso!
) else (
    echo ✅ GLFW já está presente.
)

:: 4. Gerar projeto com CMake
echo.
echo [4/6] Executando CMake...
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 (
    echo ❌ Erro durante configuração do CMake.
    cd ..
    exit /b 1
)

:: 5. Build do projeto
echo.
echo [5/6] Compilando o projeto...
cmake --build .
if errorlevel 1 (
    echo ❌ Erro na compilação!
    cd ..
    exit /b 1
)

:: 6. Cópia de assets (ex: shaders)
echo.
echo [6/6] Copiando assets para diretório de execução...
set TARGET_DIR=src
set TARGET_EXEC=vulkan_engine_app.exe
if exist %TARGET_DIR%\%TARGET_EXEC% (
    xcopy /E /I /Y "..\assets" "%TARGET_DIR%\assets"
    echo ✅ Assets copiados para %TARGET_DIR%\assets
) else (
    echo ⚠️ Não foi possível localizar o executável final para copiar assets.
)

cd ..
echo.
echo 🎉 Build concluído com sucesso!
echo 🔹 Executável: build\src\vulkan_engine_app.exe
echo 🔹 Rode direto no CLion ou via terminal!
