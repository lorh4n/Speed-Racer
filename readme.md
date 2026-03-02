# Speed Racer - Motor Gráfico Vulkan

Projeto de motor gráfico desenvolvido em C++ utilizando a API Vulkan.

## Pré-requisitos (Linux)

Como este projeto utiliza submódulos e compila dependências como o **GLFW** e **Vulkan Memory Allocator (VMA)**, você precisa garantir que o seu sistema tenha os drivers de vídeo atualizados e as ferramentas de desenvolvimento necessárias instaladas.

### 1. Ferramentas de Compilação
Instale o compilador e o CMake:
```bash
sudo apt update
sudo apt install build-essential cmake
```

### 2. Vulkan SDK e Validation Layers
Para rodar o projeto em modo **Debug**, as Validation Layers são obrigatórias:
```bash
sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers
```

### 3. Dependências de Interface e Janela (X11/GLFW)
O GLFW precisa desses cabeçalhos para ser compilado corretamente no Linux:
```bash
sudo apt install libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

### 4. Bibliotecas de Terceiros (Assimp)
Caso o projeto utilize o **Assimp** instalado no sistema:
```bash
sudo apt install libassimp-dev
```

## Como Rodar

### 1. Clonar o repositório com submódulos
Este projeto utiliza submódulos Git (como o Vulkan Memory Allocator). Certifique-se de clonar recursivamente:
```bash
git clone --recursive <URL_DO_REPOSITORIO>
```
Se já clonou e as pastas em `libs/` estiverem vazias, execute:
```bash
git submodule update --init --recursive
```

### 2. Compilar e Executar
Utilize o script automatizado na pasta raiz:
```bash
./tools/build_and_run.sh
```
