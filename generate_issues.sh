#!/bin/bash

# ==============================================================================
# Script para gerar issues no GitHub a partir do roadmap do projeto Speed Racer.
#
# PRÉ-REQUISITO:
# 1. Ter a CLI do GitHub instalada: https://cli.github.com/
#    - Em sistemas Debian/Ubuntu: `sudo apt install gh`
# 2. Estar autenticado:
#    - Rode `gh auth login` e siga as instruções.
#
# COMO USAR:
# 1. Dê permissão de execução ao script: `chmod +x generate_issues.sh`
# 2. Execute o script: `./generate_issues.sh`
# ==============================================================================

set -e

echo "🚀 Iniciando a geração de issues no GitHub para o Speed Racer Engine..."
echo "Certifique-se de que você está no repositório correto e autenticado com a CLI 'gh'."
echo ""

# --- SEMANA 1: FUNDAÇÃO ARQUITETURAL ---

echo "Gerando issues para a Semana 1..."

# Issue 1.1: Resource Management System
gh issue create --title "[S1] Implement Resource Management System (ResourceManager & BufferManager)" --body "$(cat <<'EOF'
### 🎯 Objetivo
Implementar um sistema centralizado para gerenciar o ciclo de vida de recursos Vulkan (buffers, memória) e abstrair a criação de buffers, resolvendo o problema de gerenciamento manual de recursos.

### Tarefas (ResourceManager)
- [ ] Criar `ResourceManager.hpp/cpp`
- [ ] Implementar padrão Singleton thread-safe
- [ ] Criar estrutura `Resource` base (ID, tipo, handle)
- [ ] Implementar registry com `std::unordered_map<ResourceID, Resource>`
- [ ] Adicionar métodos `registerResource()` e `releaseResource()`
- [ ] Implementar destrutor automático (RAII) para limpeza
- [ ] Adicionar logging de lifecycle de recursos
- [ ] Criar testes unitários básicos

### Tarefas (BufferManager)
- [ ] Criar `BufferManager.hpp/cpp`
- [ ] Implementar estrutura `Buffer` (VkBuffer + VkDeviceMemory + size)
- [ ] Adicionar `createBuffer()` genérico
- [ ] Implementar `findMemoryType()` helper
- [ ] Adicionar `allocateMemory()` com properties
- [ ] Criar `createStagingBuffer()` especializado
- [ ] Implementar `copyBuffer()` com command buffer
- [ ] Adicionar helpers para mapping (`mapMemory/unmapMemory`)
- [ ] Integrar com ResourceManager
- [ ] Documentar uso com exemplos

### Métricas de Sucesso
- [ ] `BufferManager` pode criar um staging buffer e copiar dados para um device buffer.
- [ ] `ResourceManager` rastreia todos os buffers criados.
- [ ] A limpeza automática de recursos funciona corretamente no final do programa.
- [ ] O código existente (triângulo) ainda compila e renderiza.
EOF
)"

# Issue 1.2: Pipeline Architecture
gh issue create --title "[S1] Implement Pipeline Architecture with PipelineBuilder" --body "$(cat <<'EOF'
### 🎯 Objetivo
Substituir o `PipelineManager` hardcoded por um `PipelineBuilder` flexível usando o Builder Pattern para facilitar a criação de múltiplos pipelines gráficos.

### Tarefas
- [ ] Criar `PipelineBuilder.hpp/cpp`
- [ ] Implementar construtor com `VkDevice`
- [ ] Adicionar métodos fluentes (retornam `*this`) para configurar:
  - [ ] Shaders (`addShader`)
  - [ ] Vertex Input (`setVertexInput`)
  - [ ] Topologia (`setTopology`)
  - [ ] Viewport e Scissor (`setViewport`)
  - [ ] Rasterizer (`setRasterizer`)
  - [ ] Multisampling (`setMultisampling`)
  - [ ] Depth/Stencil (`setDepthStencil`)
  - [ ] Blending (`setBlending`)
  - [ ] Dynamic States (`setDynamicStates`)
  - [ ] Render Pass (`setRenderPass`)
  - [ ] Descriptor Layouts (`addDescriptorLayout`)
- [ ] Implementar método `build()` que retorna uma struct `Pipeline` (contendo `VkPipeline` e `VkPipelineLayout`).
- [ ] Adicionar validação de estado para garantir que todas as configurações necessárias foram fornecidas.
- [ ] (Opcional) Criar presets para configurações comuns (e.g., `createBasicPipeline()`).
- [ ] Integrar com o `ResourceManager` para rastreamento do ciclo de vida do pipeline.

### Métricas de Sucesso
- [ ] O `PipelineBuilder` consegue criar um pipeline básico funcional.
- [ ] A API fluente é ergonômica e funciona corretamente.
- [ ] A validação de estado lança exceções para configurações incompletas.
- [ ] É trivialmente fácil criar uma variação do pipeline (e.g., wireframe).
EOF
)"

# Issue 1.3: Asset Management System
gh issue create --title "[S1] Implement Asset Management System" --body "$(cat <<'EOF'
### 🎯 Objetivo
Criar um `AssetManager` para abstrair o acesso a arquivos de assets (shaders, texturas, modelos), eliminando caminhos hardcoded e centralizando o carregamento.

### Tarefas
- [ ] Criar `AssetManager.hpp/cpp`.
- [ ] Implementar como um Singleton.
- [ ] Adicionar métodos para configurar diretórios base (`setShaderDirectory`, `setTextureDirectory`, etc.).
- [ ] Implementar métodos de resolução de paths (`getShaderPath`, etc.) que retornam o caminho completo de um asset.
- [ ] Criar um registro interno (cache) para assets já carregados (preparação para o futuro).
- [ ] Adicionar validação de existência de arquivos ao tentar resolver um path.
- [ ] Implementar logging para o carregamento de assets.
- [ ] Integrar com o `ShaderManager` existente para carregar shaders usando o `AssetManager`.

### Métricas de Sucesso
- [ ] Paths de shaders são resolvidos corretamente pelo `AssetManager`.
- [ ] Não existem mais caminhos de arquivo relativos (`../../`) ou absolutos hardcoded no código da engine.
- [ ] O sistema está preparado para o cache de assets (mesmo que o cache em si não seja totalmente implementado).
- [ ] O log de assets carregados é claro e informativo.
EOF
)"

# Issue 1.4: Integração e Refatoração
gh issue create --title "[S1] Integrate New Core Systems and Refactor VulkanManager" --body "$(cat <<'EOF'
### 🎯 Objetivo
Integrar os novos sistemas (`ResourceManager`, `BufferManager`, `PipelineBuilder`, `AssetManager`) no `VulkanManager` e refatorar a classe para reduzir seu acoplamento e complexidade.

### Tarefas de Refatoração
- [ ] Substituir a criação manual de pipeline no `VulkanManager` por uma chamada ao `PipelineBuilder`.
- [ ] Registrar todos os recursos Vulkan criados (buffers, pipelines, etc.) no `ResourceManager`.
- [ ] Usar o `AssetManager` para obter os paths de todos os shaders.
- [ ] Remover código duplicado e simplificar métodos para terem menos de 50 linhas cada, se possível.
- [ ] Adicionar comentários de documentação (`Doxygen` ou similar) para as novas APIs públicas.

### Tarefas de Teste e Validação
- [ ] Verificar que o triângulo original ainda renderiza perfeitamente após a refatoração.
- [ ] Testar o redimensionamento da janela.
- [ ] Verificar o cleanup de recursos sem memory leaks (usando as mensagens de log do `ResourceManager` ou Valgrind).
- [ ] Testar a criação de um segundo pipeline (e.g., wireframe) para garantir que o `PipelineBuilder` é flexível.

### Tarefas de Documentação
- [ ] Documentar a API pública do `ResourceManager`.
- [ ] Documentar a API pública do `BufferManager`.
- [ ] Documentar a API pública do `PipelineBuilder`.
- [ ] Documentar a API pública do `AssetManager`.
- [ ] Atualizar o `README.md` para refletir a nova arquitetura.
EOF
)"

echo "Issues da Semana 1 geradas."
echo ""
echo "Gerando issues para a Semana 2..."

# --- SEMANA 2: REAL GEOMETRY ---

# Issue 2.1: Vertex Buffer
gh issue create --title "[S2] Implement Vertex Buffers" --body "$(cat <<'EOF'
### 🎯 Objetivo
Substituir os vértices hardcoded no shader por um vertex buffer real, alimentado pela CPU e alocado na memória da GPU.

### Tarefas
- [ ] Criar a struct `Vertex` em `include/core/Vertex.hpp` (com `pos`, `color`, `uv`).
- [ ] Na struct `Vertex`, implementar os métodos estáticos `getBindingDescription()` e `getAttributeDescriptions()`.
- [ ] No código principal, criar um `std::vector<Vertex>` com os dados de um quadrado ou cubo.
- [ ] Usar o `BufferManager` para criar um vertex buffer na memória do dispositivo (`VK_BUFFER_USAGE_VERTEX_BUFFER_BIT`).
- [ ] Implementar o upload dos dados do vetor para o buffer da GPU usando um staging buffer.
- [ ] Atualizar o `PipelineBuilder` para usar as descrições de binding e atributo da struct `Vertex`.
- [ ] Modificar o vertex shader para receber os atributos de vértice (`layout(location = ...)`).
- [ ] Atualizar o comando de desenho no render loop para bindar o vertex buffer (`vkCmdBindVertexBuffers`).
- [ ] Testar a renderização do novo objeto.

### Arquivos a Criar/Modificar
- `include/core/Vertex.hpp`
- `src/core/Vertex.cpp`
- `assets/shaders/core/basic_vertex_input.vert` (novo shader)
- `src/core/VulkanManager.cpp` (ou onde a lógica de renderização estiver)
EOF
)"

# Issue 2.2: Index Buffer
gh issue create --title "[S2] Implement Index Buffers" --body "$(cat <<'EOF'
### 🎯 Objetivo
Otimizar a renderização de geometria usando um index buffer para reutilizar vértices.

### Tarefas
- [ ] Criar um `std::vector<uint32_t>` com os índices para desenhar um objeto (e.g., um quadrado com dois triângulos usa 6 índices para 4 vértices).
- [ ] Implementar a função `createIndexBuffer()` no `BufferManager`.
- [ ] Fazer o upload dos dados de índices para a GPU usando um staging buffer.
- [ ] Atualizar o command buffer para bindar o index buffer (`vkCmdBindIndexBuffer`).
- [ ] Mudar a chamada de desenho de `vkCmdDraw()` para `vkCmdDrawIndexed()`.
- [ ] Testar com um cubo, que usa 36 índices para 8 vértices, para validar a economia.
EOF
)"

# Issue 2.3: Advanced Geometry
gh issue create --title "[S2] Implement Mesh Class and Procedural Geometry" --body "$(cat <<'EOF'
### 🎯 Objetivo
Abstrair a geometria em uma classe `Mesh` e criar funções para gerar primitivas 3D de forma procedural.

### Tarefas
- [ ] Criar uma classe `Mesh` que encapsula um vertex buffer e um index buffer (e suas contagens).
- [ ] Implementar funções ou classes de geometria procedural que retornam um objeto `Mesh`:
  - [ ] `createCube()`
  - [ ] `createSphere()`
  - [ ] `createPlane()`
- [ ] Modificar a lógica de renderização para aceitar e desenhar um objeto `Mesh`.
- [ ] Adicionar múltiplos objetos `Mesh` à cena para serem renderizados.
- [ ] (Opcional) Otimizar o processo de upload, talvez usando um "batch" para múltiplos staging buffers.
- [ ] Fazer um profiling de performance básico.
- [ ] Documentar a classe `Mesh` e as funções de geometria.
EOF
)"

echo "Issues da Semana 2 geradas."
echo ""
echo "✅ Geração de issues concluída!"
echo "Para criar as issues no seu repositório GitHub, siga estes passos no seu terminal:"
echo "1. Dê permissão de execução ao script: chmod +x generate_issues.sh"
echo "2. Execute o script: ./generate_issues.sh"
