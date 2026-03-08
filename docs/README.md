# 🏁 Speed Racer — Documentação Técnica

Bem-vindo à documentação oficial do **Speed Racer**, uma engine gráfica experimental desenvolvida em **Vulkan**. Este documento serve como guia para entender a estrutura, os módulos e o fluxo de dados do projeto.

---

## 🏗️ Estrutura da Documentação

1.  [**Arquitetura do Motor**](Architecture.md)  
    Uma visão geral de como as classes interagem e o fluxo de inicialização.
2.  [**Referência de Módulos (Core)**](Core_Modules.md)  
    Detalhamento técnico de cada classe em `src/core` e `include/core`.
3.  [**Gerenciamento de Recursos**](Resources.md)  
    Explicação sobre Buffers, VMA (Vulkan Memory Allocator) e o sistema de Handles.
4.  [**Pipeline de Renderização**](Rendering.md)  
    Detalhes sobre o RenderPass, Shaders, Swapchain e o loop de desenho.
5.  [**Diário de Desenvolvimento**](Diary.md)  
    O histórico de decisões e evolução técnica do projeto.

---

## 🚀 Tecnologias Utilizadas

-   **Linguagem:** C++17
-   **API Gráfica:** Vulkan 1.0+
-   **Gerenciamento de Janelas:** GLFW 3.3
-   **Matemática Linear:** GLM
-   **Memória:** Vulkan Memory Allocator (VMA)
-   **Carregamento de Modelos:** Assimp (Em implementação)

---

## 📂 Organização do Código

-   `include/core/`: Cabeçalhos principais do motor.
-   `src/core/`: Implementação da lógica de baixo nível.
-   `src/app/`: Ponto de entrada (Main).
-   `assets/shaders/`: Código fonte dos Shaders (GLSL) e binários compilados (SPIR-V).
-   `libs/`: Dependências externas gerenciadas via Git ou CMake.
