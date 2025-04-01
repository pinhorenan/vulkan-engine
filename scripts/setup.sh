#!/bin/bash

echo "==========================================="
echo "🚀 Vulkan Engine - Setup & Build (Linux/macOS)"
echo "==========================================="

# 1. Verificar Vulkan SDK
echo
echo "[1/6] Verificando Vulkan SDK..."
if [ -z "$VULKAN_SDK" ]; then
  echo "❌ Vulkan SDK não está configurado!"
  echo "👉 Exporte a variável de ambiente VULKAN_SDK ou execute via terminal configurado pelo SDK."
  exit 1
else
  echo "✅ Vulkan SDK detectado em: $VULKAN_SDK"
fi

# 2. Criar estrutura de diretórios
echo
echo "[2/6] Criando estrutura de diretórios..."
mkdir -p build
mkdir -p external
mkdir -p assets/shaders
echo "✔️  build/, external/, assets/shaders"

# 3. Clonar GLFW se necessário
echo
echo "[3/6] Verificando GLFW..."
if [ ! -d "external/glfw" ]; then
  echo "⏳ Clonando GLFW..."
  git clone --depth=1 https://github.com/glfw/glfw external/glfw
  if [ $? -ne 0 ]; then
    echo "❌ Falha ao clonar GLFW!"
    exit 1
  fi
  echo "✅ GLFW clonado!"
else
  echo "✅ GLFW já presente."
fi

# 4. Executar CMake
echo
echo "[4/6] Executando CMake..."
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Debug
if [ $? -ne 0 ]; then
  echo "❌ Erro na configuração CMake"
  cd ..
  exit 1
fi

# 5. Build do projeto
echo
echo "[5/6] Compilando o projeto..."
cmake --build .
if [ $? -ne 0 ]; then
  echo "❌ Erro na compilação"
  cd ..
  exit 1
fi

# 6. Copiar assets
echo
echo "[6/6] Copiando assets para diretório de execução..."
if [ -f "src/vulkan_engine_app" ]; then
  cp -r ../assets ./src/assets
  echo "✅ Assets copiados para src/assets"
else
  echo "⚠️ Executável não encontrado para cópia de assets."
fi

cd ..
echo
echo "🎉 Build concluído com sucesso!"
echo "🔹 Executável: build/src/vulkan_engine_app"
