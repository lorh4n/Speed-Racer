# 🎨 Detalhes da Renderização

O sistema de desenho do Speed Racer é projetado para minimizar a sobrecarga da CPU, pré-gravando o máximo de estado possível.

---

## 🏗️ O Pipeline Gráfico (`PipelineManager`)

O pipeline é o "estado" da GPU. Ele define:
-   **Shaders**: Quais arquivos SPIR-V serão executados.
-   **Vertex Input**: Como os dados de `Vertex` (pos, color) são lidos da memória.
-   **Input Assembly**: Se vamos desenhar triângulos, linhas ou pontos.
-   **Rasterizer**: Se vamos desenhar o preenchimento ou apenas o aramado (wireframe).
-   **Color Blending**: Como as cores se misturam (transparência).

---

## 📝 Gestão de Comandos (`CommandManager`)

O Vulkan não executa comandos instantaneamente. Eles precisam ser gravados em um `VkCommandBuffer`.

### Pool de Comandos
-   Cada thread que grava comandos deve ter seu próprio `VkCommandPool`.
-   No Speed Racer, o `CommandManager` centraliza a alocação desses buffers.

### Gravação por Frame
-   O motor utiliza `MAX_FRAMES_IN_FLIGHT = 2`.
-   Isso significa que enquanto a GPU está desenhando o Frame A, a CPU já pode estar gravando os comandos do Frame B. Isso evita que o processador fique parado esperando a placa de vídeo.

---

## ⚡ Abstração de Desenho (`Mesh`)

A classe `Mesh` simplifica o loop de renderização. Em vez de chamar funções complexas do Vulkan no `VulkanManager`, usamos:

1.  **`mesh->bind(cmd)`**:
    -   Internamente chama `vkCmdBindVertexBuffers` e `vkCmdBindIndexBuffer`.
    -   Configura a GPU para ler os dados específicos desse objeto.
2.  **`mesh->draw(cmd)`**:
    -   Chama `vkCmdDrawIndexed`.
    -   Envia o comando final de processamento para a GPU.

---

## 🎥 Transformações e Push Constants

Para mover objetos (rotação, translação), enviamos uma matriz de 4x4 (64 bytes) via Push Constants.

```cpp
// Exemplo de envio no recordCommandBuffer
MeshPushConstants constants;
constants.render_matrix = projection * view * model;

vkCmdPushConstants(
    commandBuffer, 
    graphicsPipelineLayout, 
    VK_SHADER_STAGE_VERTEX_BIT, 
    0, 
    sizeof(MeshPushConstants), 
    &constants
);
```
Essa matriz é multiplicada pela posição do vértice dentro do **Vertex Shader** (`basic.vert`).
