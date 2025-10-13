# 🏎️ Speed Racer Engine - Development Roadmap & Checklist

```
   _____ ____  ________________     ____  ___   ____________ 
  / ___// __ \/ ____/ ____/ __ \   / __ \/   | / ____/ ____/ 
  \__ \/ /_/ / __/ / __/ / / / /  / /_/ / /| |/ /   / __/    
 ___/ / ____/ /___/ /___/ /_/ /  / _, _/ ___ / /___/ /___    
/____/_/   /_____/_____/_____/  /_/ |_/_/  |_\____/_____/    
                                                              
            🏁 VULKAN GAME ENGINE ROADMAP 🏁
```

---

## 📍 ESTADO ATUAL (25% Completo)

```
┌─────────────────────────────────────────────────────────────┐
│ ████████████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  25% │
└─────────────────────────────────────────────────────────────┘
   Hello    Vertex   Uniforms  Textures  Models   Full
 Triangle   Buffers                              Engine
    ↑ VOCÊ ESTÁ AQUI
```

### ✅ Implementado (Fundação Sólida)
- [x] Instância Vulkan com validation layers
- [x] Physical device selection com queue families
- [x] Logical device creation
- [x] Surface e Window management (GLFW)
- [x] Swapchain com image views e recreation
- [x] Graphics pipeline completa
- [x] Render pass básico
- [x] Framebuffers
- [x] Command buffers e command pool
- [x] Sincronização (semaphores e fences)
- [x] Rendering loop funcional
- [x] **Triângulo colorido renderizado** 🎨

---

## 🗺️ ARQUITETURA VISUAL ATUAL

```
┌─────────────────────────────────────────────────────────┐
│                   VulkanManager                         │
│              (Coordenador Central)                      │
│  ┌───────────────────────────────────────────────────┐ │
│  │ • VkInstance                                      │ │
│  │ • VkDevice                                        │ │
│  │ • VkPhysicalDevice                                │ │
│  │ • VkSurface                                       │ │
│  │ • Render Loop                                     │ │
│  └───────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────┘
          │         │         │         │         │
    ┌─────┴───┬─────┴───┬─────┴───┬─────┴───┬─────┴────┐
    │         │         │         │         │          │
┌───▼───┐ ┌──▼──┐  ┌───▼────┐ ┌──▼───┐ ┌───▼───┐  ┌──▼───┐
│Window │ │Swap │  │Pipeline│ │Render│ │Command│  │Queue │
│Manager│ │chain│  │Manager │ │Pass  │ │Manager│  │Mgr   │
└───────┘ └─────┘  └────────┘ └──────┘ └───────┘  └──────┘
  (GLFW)   (Images)  (Static)  (Static) (Commands) (Queues)
```

### ⚠️ Problemas da Arquitetura Atual
```
❌ VulkanManager conhece TODOS os detalhes (3000+ linhas esperadas)
❌ Difícil criar múltiplos pipelines/objetos
❌ Recursos espalhados sem gerenciamento centralizado
❌ Pouca reutilização de código
❌ Acoplamento moderado/alto
```

---

## 🎯 ROADMAP COMPLETO (4 Semanas + Futuro)

```
╔══════════════════════════════════════════════════════════════╗
║           FASE 1: FUNDAÇÃO ARQUITETURAL (SEMANA 1)          ║
║                    🏗️ 4-5 DIAS                              ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 1: Core Engine Architecture

#### 🎯 Objetivo
Criar sistemas fundamentais que facilitarão TODAS as features futuras, evitando refatoração massiva depois.

---

#### **DIA 1-2: Resource Management System** ⏱️ 12-16h

##### 📋 Tarefas

**1. ResourceManager (Gerenciador Central)**
```cpp
// Estrutura Mental:
ResourceManager (Singleton)
├── Resource Registry (std::unordered_map)
│   ├── VkBuffer tracking
│   ├── VkImage tracking
│   ├── VkPipeline tracking
│   └── VkDescriptorSet tracking
├── Ownership Management
│   ├── Automatic cleanup
│   └── Resource lifetime tracking
└── Query Interface
    ├── getBuffer(id)
    ├── getImage(id)
    └── releaseResource(id)
```

**Checklist de Implementação:**
- [ ] Criar `ResourceManager.hpp/cpp`
- [ ] Implementar padrão Singleton thread-safe
- [ ] Criar estrutura `Resource` base (ID, tipo, handle)
- [ ] Implementar registry com `std::unordered_map<ResourceID, Resource>`
- [ ] Adicionar métodos `registerResource()` e `releaseResource()`
- [ ] Implementar destrutor automático (RAII)
- [ ] Adicionar logging de lifecycle de recursos
- [ ] Criar testes unitários básicos

**Arquivos a Criar:**
```
include/core/ResourceManager.hpp
src/core/ResourceManager.cpp
include/core/Resource.hpp  (struct base)
```

---

**2. BufferManager (Abstração de Buffers)**
```cpp
// Estrutura Mental:
BufferManager
├── Buffer Creation
│   ├── createBuffer(size, usage)
│   ├── createStagingBuffer(data)
│   └── createUniformBuffer(size)
├── Memory Management
│   ├── allocateMemory(buffer, properties)
│   ├── findMemoryType(requirements)
│   └── Memory alignment helpers
└── Transfer Operations
    ├── uploadToBuffer(staging → device)
    ├── copyBuffer(src → dst)
    └── Command buffer helpers
```

**Checklist de Implementação:**
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

**Arquivos a Criar:**
```
include/core/BufferManager.hpp
src/core/BufferManager.cpp
include/core/Buffer.hpp  (struct Buffer)
```

**Diagramas de Uso:**
```
┌─────────────┐
│   Client    │
└──────┬──────┘
       │ createVertexBuffer(vertices)
       ▼
┌─────────────────┐    1. Cria staging    ┌──────────┐
│ BufferManager   │───────────────────────►│ Staging  │
│                 │                        │  Buffer  │
└─────────────────┘    2. Cria device      └──────────┘
       │               buffer
       ▼                                   ┌──────────┐
┌─────────────────┐    3. Copy staging    │  Device  │
│ CommandManager  │───────────────────────►│  Buffer  │
│                 │    to device           └──────────┘
└─────────────────┘
       │
       ▼
┌─────────────────┐    4. Registra        ┌──────────────┐
│ResourceManager  │───────────────────────►│   Registry   │
└─────────────────┘                        └──────────────┘
```

**Métricas de Sucesso Dia 1-2:**
- ✅ BufferManager pode criar staging buffer
- ✅ BufferManager pode copiar dados para device buffer
- ✅ ResourceManager rastreia todos os buffers criados
- ✅ Cleanup automático funciona corretamente
- ✅ Código existente (triângulo) ainda compila

---

#### **DIA 3: Pipeline Architecture** ⏱️ 6-8h

##### 📋 Tarefas

**1. PipelineBuilder (Builder Pattern)**
```cpp
// Estrutura Mental (Fluent API):
PipelineBuilder builder(device);
auto pipeline = builder
    .addShader(VERTEX, "shader.vert.spv")
    .addShader(FRAGMENT, "shader.frag.spv")
    .setVertexInput(vertexDesc, attributeDesc)
    .setTopology(TRIANGLE_LIST)
    .setViewport(extent)
    .setRasterizer(FILL, BACK_CULL)
    .setMultisampling(MSAA_OFF)
    .setBlending(ALPHA_BLEND)
    .setRenderPass(renderPass)
    .build();
```

**Checklist de Implementação:**
- [ ] Criar `PipelineBuilder.hpp/cpp`
- [ ] Implementar construtor com VkDevice
- [ ] Adicionar métodos fluentes (retornam `*this`)
  - [ ] `addShader(stage, path)`
  - [ ] `setVertexInput(bindings, attributes)`
  - [ ] `setTopology(topology)`
  - [ ] `setViewport(extent)`
  - [ ] `setRasterizer(polygonMode, cullMode)`
  - [ ] `setMultisampling(samples)`
  - [ ] `setDepthStencil(enable)`
  - [ ] `setBlending(mode)`
  - [ ] `setDynamicStates(states)`
  - [ ] `setRenderPass(renderPass)`
  - [ ] `addDescriptorLayout(layout)`
- [ ] Implementar `build()` → retorna `Pipeline` struct
- [ ] Adicionar validação de estado (throw se configs faltando)
- [ ] Criar presets comuns (`createBasicPipeline()`, `createWireframePipeline()`)
- [ ] Integrar com ResourceManager

**Arquivos a Criar:**
```
include/core/PipelineBuilder.hpp
src/core/PipelineBuilder.cpp
include/core/Pipeline.hpp  (struct Pipeline)
```

**Diagrama de Builder Pattern:**
```
┌────────────────────────────────────────────────────────┐
│              PipelineBuilder State                     │
├────────────────────────────────────────────────────────┤
│  shaders: [vertex, fragment]                           │
│  vertexInput: {bindings, attributes}                   │
│  topology: TRIANGLE_LIST                               │
│  viewport: {width, height}                             │
│  rasterizer: {FILL, BACK_CULL}                         │
│  multisampling: DISABLED                               │
│  blending: DISABLED                                    │
│  renderPass: VkRenderPass                              │
│  descriptorLayouts: []                                 │
└────────────────────────────────────────────────────────┘
                     │
                     │ .build()
                     ▼
         ┌───────────────────────┐
         │  VkGraphicsPipeline   │
         │  VkPipelineLayout     │
         └───────────────────────┘
```

**Comparação Antes/Depois:**
```cpp
// ❌ ANTES (PipelineManager atual):
// - 200+ linhas hardcoded em PipelineManager::create()
// - Difícil criar variações
// - Configurações espalhadas

// ✅ DEPOIS (PipelineBuilder):
auto basic = PipelineBuilder(device)
    .addShader(VERTEX, "basic.vert")
    .addShader(FRAGMENT, "basic.frag")
    .setViewport(extent)
    .setRenderPass(renderPass)
    .build();

auto wireframe = PipelineBuilder(device)
    .addShader(VERTEX, "basic.vert")
    .addShader(FRAGMENT, "wireframe.frag")
    .setRasterizer(LINE, NO_CULL)  // Diferença!
    .setViewport(extent)
    .setRenderPass(renderPass)
    .build();
```

**Métricas de Sucesso Dia 3:**
- ✅ PipelineBuilder cria pipeline básico
- ✅ API fluente funciona corretamente
- ✅ Validação detecta configurações faltando
- ✅ Fácil criar múltiplos pipelines
- ✅ Código mais limpo que antes

---

#### **DIA 4: Asset Management System** ⏱️ 6-8h

##### 📋 Tarefas

**1. AssetManager (Gerenciamento de Assets)**
```cpp
// Estrutura Mental:
AssetManager (Singleton)
├── Path Resolution
│   ├── Shader paths
│   ├── Texture paths
│   ├── Model paths
│   └── Audio paths (futuro)
├── Asset Registry
│   ├── Loaded shaders
│   ├── Loaded textures (futuro)
│   └── Loaded models (futuro)
└── Hot-Reload Support (preparação)
    └── File watchers (futuro)
```

**Checklist de Implementação:**
- [ ] Criar `AssetManager.hpp/cpp`
- [ ] Implementar Singleton pattern
- [ ] Adicionar configuração de diretórios base
  - [ ] `setShaderDirectory(path)`
  - [ ] `setTextureDirectory(path)`
  - [ ] `setModelDirectory(path)`
- [ ] Implementar path resolution
  - [ ] `getShaderPath(filename)` → full path
  - [ ] `getTexturePath(filename)` → full path
  - [ ] `getModelPath(filename)` → full path
- [ ] Criar asset registry (cache preparado)
  - [ ] `std::unordered_map<string, ShaderData>`
  - [ ] Preparar estruturas para texturas/modelos
- [ ] Adicionar validação de existência de arquivos
- [ ] Implementar logging de asset loading
- [ ] Integrar com ShaderManager existente

**Arquivos a Criar:**
```
include/core/AssetManager.hpp
src/core/AssetManager.cpp
include/core/AssetTypes.hpp  (enums, structs)
```

**Estrutura de Diretórios de Assets:**
```
assets/
├── shaders/
│   ├── core/
│   │   ├── basic.vert
│   │   ├── basic.frag
│   │   └── compiled/
│   │       ├── basic.vert.spv
│   │       └── basic.frag.spv
│   ├── advanced/
│   └── compute/
├── textures/
│   ├── diffuse/
│   ├── normal/
│   └── ui/
├── models/
│   ├── characters/
│   ├── environment/
│   └── vehicles/
└── audio/ (futuro)
```

**Exemplo de Uso:**
```cpp
// ❌ ANTES:
auto shader = ShaderManager::loadShader(device, 
    "../../assets/shaders/core/compiled/vert.spv");  // Hardcoded!

// ✅ DEPOIS:
auto& assets = AssetManager::getInstance();
assets.setShaderDirectory("assets/shaders");
auto shader = assets.loadShader(device, "core/basic.vert.spv");
```

**Métricas de Sucesso Dia 4:**
- ✅ Paths de shaders resolvidos corretamente
- ✅ Não há mais paths hardcoded no código
- ✅ Sistema preparado para cache (futuro)
- ✅ Fácil adicionar novos assets
- ✅ Logging claro de assets carregados

---

#### **DIA 5: Integração e Refatoração** ⏱️ 8-10h

##### 📋 Tarefas Críticas

**1. Refatorar VulkanManager**
```cpp
// Meta: Reduzir acoplamento e usar novos sistemas
```

**Checklist de Refatoração:**
- [ ] Substituir criação de pipeline por PipelineBuilder
- [ ] Integrar ResourceManager para rastreamento
- [ ] Usar AssetManager para paths de shaders
- [ ] Preparar BufferManager para uso futuro
- [ ] Remover código duplicado
- [ ] Simplificar métodos (< 50 linhas cada)
- [ ] Adicionar comentários de documentação

**2. Testes de Integração**
- [ ] Verificar que triângulo ainda renderiza
- [ ] Testar window resize
- [ ] Testar minimização
- [ ] Verificar cleanup sem memory leaks (Valgrind)
- [ ] Testar criação de múltiplos pipelines
- [ ] Verificar logging detalhado

**3. Documentação**
- [ ] Documentar API do ResourceManager
- [ ] Documentar API do BufferManager
- [ ] Documentar API do PipelineBuilder
- [ ] Documentar API do AssetManager
- [ ] Criar exemplos de uso de cada sistema
- [ ] Atualizar README do projeto

**4. Commit e Limpeza**
```bash
# Estrutura de commits:
git commit -m "feat: add ResourceManager system"
git commit -m "feat: add BufferManager with staging support"
git commit -m "feat: add PipelineBuilder with fluent API"
git commit -m "feat: add AssetManager for path resolution"
git commit -m "refactor: integrate new systems into VulkanManager"
git commit -m "docs: add documentation for core systems"
```

**Código Antes vs Depois (Exemplo):**
```cpp
// ❌ ANTES (VulkanManager::createGraphicsPipeline):
void VulkanManager::createGraphicsPipeline() {
    auto vertCode = ShaderManager::readFile("../../assets/...");
    auto fragCode = ShaderManager::readFile("../../assets/...");
    // ... 200 linhas de configuração manual ...
}

// ✅ DEPOIS:
void VulkanManager::createGraphicsPipeline() {
    auto& assets = AssetManager::getInstance();
    auto& resources = ResourceManager::getInstance();
    
    auto pipelineData = PipelineBuilder(device)
        .addShader(VERTEX, assets.getShader("core/basic.vert.spv"))
        .addShader(FRAGMENT, assets.getShader("core/basic.frag.spv"))
        .setViewport(swapchainManager->getSwapchainExtent())
        .setRenderPass(renderPass)
        .build();
    
    resources.registerPipeline("basic", pipelineData);
    graphicsPipeline = pipelineData.pipeline;
    graphicsPipelineLayout = pipelineData.layout;
}
```

**Métricas de Sucesso Dia 5:**
- ✅ Código compila sem warnings
- ✅ Triângulo renderiza perfeitamente
- ✅ Todos os testes passam
- ✅ Documentação completa
- ✅ Commits organizados
- ✅ **Pronto para Semana 2!** 🚀

---

```
╔══════════════════════════════════════════════════════════════╗
║        FASE 2: VERTEX E INDEX BUFFERS (SEMANA 2)            ║
║                    🎨 3-4 DIAS                              ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 2: Real Geometry

#### 🎯 Objetivo
Substituir vértices hardcoded no shader por buffers reais na GPU.

---

#### **DIA 1: Vertex Buffer Implementation** ⏱️ 8h

**Checklist:**
- [ ] Criar struct `Vertex` (pos, color, UV)
- [ ] Implementar `getBindingDescription()`
- [ ] Implementar `getAttributeDescriptions()`
- [ ] Criar vetor de vértices hardcoded (cubo/quad)
- [ ] Usar BufferManager para criar vertex buffer
- [ ] Implementar staging buffer upload
- [ ] Atualizar PipelineBuilder com vertex input
- [ ] Modificar shaders para receber input
- [ ] Testar rendering

**Arquivos:**
```
include/core/Vertex.hpp
src/core/Vertex.cpp
assets/shaders/core/basic_vertex_input.vert
```

**Diagrama de Vertex Buffer:**
```
┌──────────────┐
│ CPU Memory   │  std::vector<Vertex>
│  [Vertices]  │
└──────┬───────┘
       │ BufferManager::createVertexBuffer()
       ▼
┌──────────────┐      ┌────────────────┐
│   Staging    │──────►│ Device Memory  │
│   Buffer     │ Copy  │ (Vertex Buffer)│
└──────────────┘      └────────────────┘
                              │
                              ▼
                       ┌──────────────┐
                       │   Pipeline   │
                       │  Input Stage │
                       └──────────────┘
```

---

#### **DIA 2: Index Buffer Implementation** ⏱️ 6h

**Checklist:**
- [ ] Criar vetor de índices
- [ ] Implementar `createIndexBuffer()` no BufferManager
- [ ] Upload de índices via staging buffer
- [ ] Atualizar command buffer recording (`vkCmdBindIndexBuffer`)
- [ ] Usar `vkCmdDrawIndexed()` ao invés de `vkCmdDraw()`
- [ ] Testar com cubo (36 índices, 8 vértices)

---

#### **DIA 3-4: Advanced Geometry + Testing** ⏱️ 10h

**Checklist:**
- [ ] Criar classe `Mesh` (vertices + indices)
- [ ] Implementar geometria procedural:
  - [ ] `createCube()`
  - [ ] `createSphere()`
  - [ ] `createPlane()`
- [ ] Adicionar múltiplos objetos na cena
- [ ] Otimizar uploads (batch staging)
- [ ] Performance profiling
- [ ] Documentação

**Resultado:** Múltiplos objetos 3D renderizados! 🎉

---

```
╔══════════════════════════════════════════════════════════════╗
║       FASE 3: UNIFORMS + TEXTURES (SEMANA 3)                ║
║                    🌐 5-6 DIAS                              ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 3: Transformations & Textures

#### **DIA 1-2: Uniform Buffers (MVP Matrices)** ⏱️ 12h

**Conceitos:**
```
Uniform Buffer Object (UBO):
- Dados constantes por draw call
- Acessíveis em shaders
- Atualizados dinamicamente

MVP Transform:
Model → World → View → Projection → Clip Space
```

**Checklist:**
- [ ] Criar struct `UniformBufferObject` (Model, View, Proj)
- [ ] Implementar `DescriptorManager`:
  - [ ] `createDescriptorPool()`
  - [ ] `createDescriptorSetLayout()`
  - [ ] `allocateDescriptorSets()`
  - [ ] `updateDescriptorSets()`
- [ ] Criar uniform buffers (um por frame in flight)
- [ ] Integrar com PipelineBuilder (descriptor layout)
- [ ] Atualizar shaders para usar UBO
- [ ] Implementar matriz de rotação simples
- [ ] Testar transformações

**Arquivos:**
```
include/core/DescriptorManager.hpp
src/core/DescriptorManager.cpp
include/core/UniformBuffer.hpp
assets/shaders/core/mvp.vert
```

**Diagrama de Descriptors:**
```
┌───────────────────┐
│ DescriptorPool    │  (Alocador de descriptor sets)
└─────────┬─────────┘
          │
          ▼
┌───────────────────┐
│DescriptorSetLayout│  (Layout: binding 0 = UBO)
└─────────┬─────────┘
          │
          ▼
┌───────────────────┐
│  DescriptorSet    │  (Instância que aponta para UBO)
│  binding 0 ───────┼──►  Uniform Buffer
└───────────────────┘
```

---

#### **DIA 3-4: Texture System** ⏱️ 14h

**Checklist:**
- [ ] Criar `TextureManager.hpp/cpp`
- [ ] Implementar `createImage()` helper
- [ ] Implementar `transitionImageLayout()` (barriers)
- [ ] Implementar `copyBufferToImage()`
- [ ] Adicionar `createSampler()`
- [ ] Integrar stb_image para carregar PNG/JPG
- [ ] Criar `loadTexture(path)` end-to-end
- [ ] Adicionar texture ao descriptor set
- [ ] Atualizar shaders para sampling
- [ ] Adicionar UVs ao struct Vertex
- [ ] Testar com textura checkerboard

**Arquivos:**
```
include/core/TextureManager.hpp
src/core/TextureManager.cpp
include/core/Image.hpp
assets/shaders/core/textured.frag
```

**Pipeline de Textura:**
```
┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐
│  Image   │───►│ Staging  │───►│  Device  │───►│ Sampler  │
│  File    │Load│  Buffer  │Copy│  Image   │Bind│          │
│ (PNG/JPG)│    │          │    │          │    │          │
└──────────┘    └──────────┘    └──────────┘    └──────────┘
                                      │
                                      ▼
                            ┌─────────────────┐
                            │ Descriptor Set  │
                            │ binding 1=image │
                            │ binding 2=sampler│
                            └─────────────────┘
```

---

#### **DIA 5-6: Camera System** ⏱️ 10h

**Checklist:**
- [ ] Criar `Camera.hpp/cpp`
- [ ] Implementar `lookAt()` (view matrix)
- [ ] Implementar `perspective()` (projection matrix)
- [ ] Adicionar controles (WASD + Mouse)
- [ ] Integrar com input system (GLFW callbacks)
- [ ] Atualizar UBO com camera matrices
- [ ] Testar movimento livre
- [ ] Adicionar câmera FPS

**Resultado:** Objetos texturizados com câmera livre! 🎥

---

```
╔══════════════════════════════════════════════════════════════╗
║         FASE 4: DEPTH + MODEL LOADING (SEMANA 4)            ║
║                    🗿 5-6 DIAS                              ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 4: Depth Testing & 3D Models

#### **DIA 1-2: Depth Buffering** ⏱️ 12h

**Checklist:**
- [ ] Encontrar formato de depth (VK_FORMAT_D32_SFLOAT)
- [ ] Criar depth image (TextureManager)
- [ ] Criar depth image view
- [ ] Atualizar `RenderPassManager::createRenderPassWithDepth()`
- [ ] Adicionar depth attachment ao render pass
- [ ] Configurar depth testing no pipeline
- [ ] Atualizar framebuffers com depth attachment
- [ ] Testar overlapping geometry
- [ ] Adicionar clear depth value

**Arquivos:**
```
src/core/RenderPassManager.cpp  (atualizar)
include/core/DepthResources.hpp
```

**Render Pass com Depth:**
```
RenderPass Attachments:
├── Attachment 0: Color (Swapchain Image)
│   ├── loadOp: CLEAR
│   └── storeOp: STORE
└── Attachment 1: Depth
    ├── loadOp: CLEAR (1.0)
    ├── storeOp: DONT_CARE
    └── format: D32_SFLOAT
```

---

#### **DIA 3-5: Model Loading System** ⏱️ 20h

**Checklist:**
- [ ] Integrar Assimp (já no CMakeLists)
- [ ] Criar `ModelLoader.hpp/cpp`
- [ ] Implementar `loadOBJ(path)` → retorna Mesh
- [ ] Criar classe `Model` (collection of Meshes)
- [ ] Implementar `Material` struct básico
- [ ] Carregar texturas associadas ao modelo
- [ ] Criar `MeshManager` para gerenciar múltiplos meshes
- [ ] Otimizar: compartilhar vértices/texturas
- [ ] Testar com modelo simples (.obj)
- [ ] Testar com modelo complexo (100k+ polígonos)

**Arquivos:**
```
include/core/ModelLoader.hpp
src/core/ModelLoader.cpp
include/core/Model.hpp
include/core/Material.hpp
include/core/MeshManager.hpp
```

**Hierarquia de Scene:**
```
Scene
├── Model 1 (Car)
│   ├── Mesh (Body)
│   │   ├── Vertices
│   │   ├── Indices
│   │   └── Material (Red Paint)
│   │       ├── Diffuse Texture
│   │       └── Normal Map
│   └── Mesh (Wheels)
│       └── Material (Rubber)
└── Model 2 (Ground)
    └── Mesh (Plane)
        └── Material (Asphalt)
```

---

#### **DIA 6: Polish & Optimization** ⏱️ 8h

**Checklist:**
- [ ] Profile performance (FPS, frame time)
- [ ] Identificar gargalos (CPU/GPU) com ferramentas de profiling
- [ ] Refatorar código crítico para otimização
- [ ] Validar uso de memória com Valgrind ou sanitizers
- [ ] Documentar a arquitetura do `ModelLoader` e `MeshManager`
- [ ] **Engine pronta para renderizar cenas 3D complexas!** 🏆

---

```
╔══════════════════════════════════════════════════════════════╗
║      FASE 5: ILUMINAÇÃO E MATERIAIS (SEMANA 5-6)            ║
║                    💡 8-10 DIAS                             ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 5-6: Lighting & Materials

#### 🎯 Objetivo
Dar vida à cena com um sistema de iluminação e materiais que reagem à luz, saindo do modo "full-bright".

---

#### **DIA 1-3: Sistema de Iluminação** ⏱️ 16h

**Checklist:**
- [ ] Adicionar normais ao `Vertex.hpp` e ao carregador de modelos
- [ ] Criar structs para Luzes (Direcional, Pontual, Spot)
- [ ] Criar um UBO para luzes e enviá-lo aos shaders
- [ ] Implementar shader de iluminação (Gouraud ou Phong)
- [ ] Adicionar objetos de luz na cena para visualização
- [ ] Testar múltiplos tipos de luzes interagindo

---

#### **DIA 4-6: Sistema de Materiais** ⏱️ 20h

**Checklist:**
- [ ] Criar struct `Material` (cores ambiente, difusa, especular; shininess)
- [ ] Integrar materiais ao `ModelLoader` (lendo arquivos .mtl)
- [ ] Criar um UBO ou Push Constants para dados de material
- [ ] Modificar shaders para usar propriedades do material
- [ ] Adicionar suporte para mapas especulares
- [ ] Testar modelos com diferentes materiais

---

```
╔══════════════════════════════════════════════════════════════╗
║       FASE 6: CENA E INTERATIVIDADE (SEMANA 7-8)            ║
║                    🌳 8-10 DIAS                             ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 7-8: Scene & Interactivity

#### 🎯 Objetivo
Estruturar a lógica de jogo com um grafo de cena e um sistema de componentes, preparando para a interatividade real.

---

#### **DIA 1-4: Scene Graph e Component System** ⏱️ 24h

**Checklist:**
- [ ] Criar classe `GameObject` ou `SceneNode`
- [ ] Implementar transformações hierárquicas (pai/filho)
- [ ] Criar uma interface `Component` base
- [ ] Implementar componentes: `TransformComponent`, `RenderComponent`, `LightComponent`
- [ ] Refatorar o render loop para iterar sobre o grafo de cena
- [ ] Criar uma classe `Scene` para gerenciar os GameObjects

---

#### **DIA 5-6: Sistema de Input Abstrato** ⏱️ 12h

**Checklist:**
- [ ] Criar `InputManager` para abstrair callbacks do GLFW
- [ ] Mapear ações (e.g., "MoverFrente", "Pular") para teclas/botões
- [ ] Criar `PlayerControllerComponent` que usa o `InputManager`
- [ ] Implementar lógica de movimento do jogador ou de um objeto controlável
- [ ] Testar controle de câmera e objetos via componentes

---

```
╔══════════════════════════════════════════════════════════════╗
║         FASE 7: RENDERIZAÇÃO AVANÇADA (FUTURO)              ║
║                    ✨ LONGO PRAZO                             ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 Futuro Próximo: Advanced Rendering

- [ ] **Shadow Mapping:** Implementar mapeamento de sombras para luzes direcionais e pontuais.
- [ ] **Skybox/Environment Mapping:** Renderizar um céu e usar cubemaps para reflexos.
- [ ] **Post-Processing:** Criar um sistema para efeitos de pós-processamento (Bloom, SSAO, FXAA).
- [ ] **Instanced Rendering:** Otimizar a renderização de um grande número de objetos idênticos.
- [ ] **Physically Based Rendering (PBR):** Migrar para um fluxo de trabalho de materiais PBR (Metálico/Rugosidade) para um visual mais realista.

---

```
╔══════════════════════════════════════════════════════════════╗
║           FASE 8: GAME ENGINE CORE (FUTURO)                 ║
║                    🚀 LONGO PRAZO                             ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 Futuro Distante: Core Game Engine Features

- [ ] **Integração com Física:** Adicionar um motor de física como Bullet ou PhysX.
- [ ] **Sistema de Animação:** Implementar animação esquelética.
- [ ] **Sistema de Áudio:** Integrar uma biblioteca de áudio como OpenAL ou FMOD.
- [ ] **UI/GUI:** Usar ImGui (já presente em `libs`) para criar menus, debug e HUDs.
- [ ] **Serialização de Cena:** Implementar a capacidade de salvar e carregar cenas em arquivos.
- [ ] **Scripting:** Integrar uma linguagem de script como Lua para a lógica de jogo.
- [ ] **Construção do Jogo:** Finalmente, usar a engine para construir o jogo **Speed Racer**! 🏎️💨