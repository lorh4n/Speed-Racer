# 🛠️ Referência de Módulos (Core)

Este documento detalha cada classe e utilitário presente no diretório `core/`, descrevendo sua função no motor.

---

## 🏗️ Gestão do Vulkan (Core)

### `VulkanManager`
-   **Função**: Orquestrador principal. Inicializa a API, mantém o loop de eventos e garante a destruição segura de todos os subsistemas.
-   **Responsabilidades**: Coordena o fluxo entre Janela, Dispositivos, Swapchain e Managers de recursos.

### `LogicalDeviceCreator` & `PhysicalDeviceSelector`
-   **Função**: Abstraem a complexidade de escolher a GPU e criar o dispositivo lógico.
-   **Destaque**: O `PhysicalDeviceSelector` verifica suporte a extensões (como Swapchain) e famílias de filas antes de selecionar o hardware.

### `QueueManager`
-   **Função**: Gerencia as filas de execução da GPU (Graphics, Present, Compute, Transfer).
-   **Responsabilidades**: Localiza os índices das famílias de filas e fornece os handles `VkQueue` necessários para submissão de comandos.

---

## 🖼️ Pipeline e Gráficos

### `SwapchainManager`
-   **Função**: Gerencia o "duplo/triplo buffering" da tela.
-   **Métodos Chave**:
    -   `recreateSwapchain`: Chamado automaticamente quando a janela é redimensionada.
    -   `createFramebuffers`: Cria os anexos de memória para o Render Pass.

### `PipelineManager`
-   **Função**: Fábrica de `VkPipeline`.
-   **Configuração**: Utiliza a struct `PipelineConfig` para definir shaders, topologia e modos de polígono sem expor toda a estrutura complexa do Vulkan.

### `RenderPassManager`
-   **Função**: Define como a GPU deve tratar os buffers de cor e profundidade no início e fim de cada operação de desenho.

### `ShaderManager`
-   **Função**: Utilitário para carregar arquivos SPIR-V (`.spv`) e criar `VkShaderModule`.

---

## 📦 Sistema de Recursos e Memória

### `ResourceManager`
-   **Função**: O proprietário (owner) real da memória.
-   **Mecânica**: Utiliza um `unordered_map` para guardar `VmaBuffer` e um `HandleAllocator` para gerar IDs únicos.

### `BufferManager`
-   **Função**: Fachada (Facade) para o programador criar buffers de alto nível.
-   **Destaque**: Implementa `executeOneTimeCommands` para realizar operações síncronas na GPU (como upload de dados via Staging Buffers).

### `Mesh` & `MeshFactory`
-   **Função**: Abstração de geometria.
-   **`Mesh`**: Encapsula os buffers de vértices/índices e fornece `bind()` e `draw()`.
-   **`MeshFactory`**: Namespace com funções utilitárias para gerar dados de formas primitivas (Cubo, Triângulo, Quad).

### `VmaWrapper`
-   **Função**: Singleton/Wrapper para o **Vulkan Memory Allocator**. Garante que o alocador seja destruído apenas após o `VkDevice` estar ocioso.

---

## 🛠️ Utilitários

### `WindowManager`
-   **Função**: Abstração da biblioteca **GLFW**. Lida com a criação da superfície (`VkSurfaceKHR`) e callbacks de redimensionamento.

### `ScopedBuffer`
-   **Função**: Utilitário **RAII** para buffers temporários que precisam ser limpos automaticamente ao saírem do escopo.

### `VulkanTools`
-   **Função**: Namespace com funções auxiliares para **Validation Layers**, Debug Messenger e seleção de extensões de instância.
