# 🏎️ Speed Racer Engine - Development Roadmap v2.0 (REVISADO)

```
   _____ ____  ________________     ____  ___   ____________ 
  / ___// __ \/ ____/ ____/ __ \   / __ \/   | / ____/ ____/ 
  \__ \/ /_/ / __/ / __/ / / / /  / /_/ / /| |/ /   / __/    
 ___/ / ____/ /___/ /___/ /_/ /  / _, _/ ___ / /___/ /___    
/____/_/   /_____/_____/_____/  /_/ |_/_/  |_\____/_____/    
                                                              
         🏁 VULKAN GAME ENGINE ROADMAP (REVISADO) 🏁
```

---

## 📝 REVISÃO CRÍTICA E AJUSTES

### ✅ O que foi mantido (estava bom)
- Diagnóstico do problema arquitetural (god-object VulkanManager)
- Divisão em semanas com entregáveis concretos
- Checklists detalhados por dia
- Sequência gráfico → dados → câmera → profundidade → modelos

### 🔄 MUDANÇAS CRÍTICAS APLICADAS

#### 1. **❌ SINGLETONS REMOVIDOS → ✅ DEPENDENCY INJECTION**
```cpp
// ❌ ANTES (Singleton):
auto& resources = ResourceManager::getInstance();

// ✅ AGORA (Injeção de Dependência):
class VulkanManager {
    ResourceManager resourceManager;     // Owned instance
    BufferManager bufferManager;
    AssetManager assetManager;
};

// Passar referências explícitas
bufferManager.initialize(device, resourceManager);
```

**Razão:** Singletons criam acoplamento oculto e dificultam testes. Injeção explícita é mais flexível.

#### 2. **🆕 VULKAN MEMORY ALLOCATOR (VMA) OBRIGATÓRIO**
```cpp
// Adicionar LOGO no Dia 1-2 da Semana 1
VmaAllocator allocator;  // Substitui gerenciamento manual de VkDeviceMemory
```

**Razão:** Gerenciar memória Vulkan manualmente é complexo e propenso a erros. VMA resolve subalocação, mapeamento e performance.

#### 3. **🆕 SHADER TOOLCHAIN NO CMAKE**
```cmake
# CMakeLists.txt - Adicionar target de shaders
add_custom_target(compile_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIR}
    COMMAND glslc ${SHADER_SOURCE} -o ${SHADER_OUTPUT}
    DEPENDS ${SHADER_SOURCES}
)
add_dependencies(Speed_Racer compile_shaders)
```

**Razão:** Shaders devem ser versionados e compilados automaticamente no build.

#### 4. **🆕 FRAMES-IN-FLIGHT VALIDADO FORMALMENTE**
```cpp
// Critério de aceite: DEVE funcionar com 2-3 frames in flight sem deadlocks
const int MAX_FRAMES_IN_FLIGHT = 2;  // Já existe
// + Validação explícita de swapchain recreation
```

#### 5. **🆕 SCENE GRAPH BÁSICO ANTECIPADO (Semana 2)**
```cpp
// Introduzir APÓS Vertex/Index Buffers
struct SceneNode {
    Transform transform;
    Renderable* renderable;  // nullptr se não renderiza
    std::vector<SceneNode*> children;
};
```

**Razão:** Evita retrabalho ao migrar de "desenhar N objetos" para "iterar cena".

#### 6. **🆕 MINI FRAMEGRAPH (Semana 2)**
```cpp
// Estrutura simples de passes
struct RenderPass {
    std::string name;
    std::function<void(VkCommandBuffer)> execute;
    std::vector<std::string> dependencies;
};
```

**Razão:** Facilita adicionar sombras/pós-processamento sem reescrever loop de render.

---

## 🗺️ ROADMAP REVISADO (4 Semanas + Futuro)

```
╔══════════════════════════════════════════════════════════════╗
║         FASE 1: FUNDAÇÃO ARQUITETURAL (SEMANA 1)            ║
║                    🏗️ 5-6 DIAS                              ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 1: Core Engine Architecture (REVISADO)

#### 🎯 Objetivo
Criar infraestrutura robusta com gerenciamento de memória profissional e build system completo.

---

#### **DIA 1-2: Memory Management + Resource System** ⏱️ 14-18h

##### 📋 Tarefas (REORDENADAS)

**1. Integrar Vulkan Memory Allocator (VMA)** 🆕
```cpp
// PRIORIDADE MÁXIMA - Fazer ANTES de BufferManager
```

**Checklist:**
- [x] Adicionar VMA ao projeto (git submodule ou download)
- [x] Atualizar CMakeLists.txt com VMA
- [x] Criar `VmaAllocator` em `VulkanManager::initVulkan()`
- [] Implementar wrapper `VmaBuffer` (VkBuffer + VmaAllocation)
- [ ] Testar criação/destruição de buffer simples
- [ ] Validar com Validation Layers

**Arquivos:**
```
libs/VulkanMemoryAllocator/  (submodule ou source)
include/core/VmaWrapper.hpp
src/core/VmaWrapper.cpp
```

---

**2. ResourceManager (COM INJEÇÃO, NÃO SINGLETON)** 🔄

```cpp
// Estrutura Mental (SEM getInstance()):
class ResourceManager {
public:
    ResourceManager(VkDevice device, VmaAllocator allocator);
    
    // Handle-based API (não expõe VkBuffer diretamente)
    BufferHandle createBuffer(const BufferCreateInfo& info);
    void destroyBuffer(BufferHandle handle);
    VkBuffer getVkBuffer(BufferHandle handle) const;
    
private:
    VkDevice device;
    VmaAllocator allocator;
    std::unordered_map<BufferHandle, VmaBuffer> buffers;
    HandleAllocator<BufferHandle> handleGen;
};
```

**Checklist:**
- [x] Criar `ResourceManager.hpp/cpp` (instância normal, NÃO singleton)
- [x] Implementar `Handle` types (`BufferHandle`, `ImageHandle`)
- [x] Criar `HandleAllocator` template (gera IDs únicos)
- [ ] Registry de recursos com handles
- [ ] Métodos `create/destroy/get` para buffers
- [ ] RAII wrappers (`ScopedBuffer`, `ScopedImage`)
- [ ] Logging de lifecycle
- [ ] Testes unitários (criar 100 buffers, verificar cleanup)

**Arquivos:**
```
include/core/ResourceManager.hpp
src/core/ResourceManager.cpp
include/core/Handle.hpp  (templates)
include/core/ResourceTypes.hpp
```

**Exemplo de Uso:**
```cpp
// ❌ ANTES (Singleton):
auto& rm = ResourceManager::getInstance();

// ✅ AGORA (Injeção):
class VulkanManager {
    ResourceManager resourceManager;  // Owned
public:
    VulkanManager() : resourceManager(device, vmaAllocator) {}
};
```

---

**3. BufferManager (usando VMA + ResourceManager)**

```cpp
class BufferManager {
public:
    BufferManager(VkDevice device, 
                  VmaAllocator allocator,
                  ResourceManager& resources,  // Injeção!
                  CommandManager& commands);
    
    BufferHandle createVertexBuffer(const void* data, size_t size);
    BufferHandle createIndexBuffer(const void* data, size_t size);
    BufferHandle createUniformBuffer(size_t size);
    
    void uploadToBuffer(BufferHandle handle, const void* data, size_t size);
private:
    VmaAllocator allocator;
    ResourceManager& resources;
    CommandManager& commands;
};
```

**Checklist:**
- [ ] Criar `BufferManager.hpp/cpp`
- [ ] Implementar `createBuffer()` usando VMA
- [ ] Staging buffer automático (VMA_MEMORY_USAGE_CPU_TO_GPU)
- [ ] `copyBuffer()` com command buffer one-time-submit
- [ ] `mapBuffer/unmapBuffer` helpers
- [ ] Integração com ResourceManager (retorna handles)
- [ ] Testes: vertex buffer, index buffer, staging

**Métricas de Sucesso Dia 1-2:**
- ✅ VMA integrado e funcionando
- ✅ BufferManager cria staging + device buffer via VMA
- ✅ ResourceManager rastreia buffers com handles
- ✅ Código existente (triângulo) ainda compila
- ✅ **Nenhum memory leak detectado** (Validation Layers)

---

#### **DIA 3: Shader Build System + PipelineBuilder** ⏱️ 8-10h

##### 📋 Tarefas (ORDEM REVISADA)

**1. Shader Toolchain no CMake** 🆕

**Checklist:**
- [ ] Criar `cmake/CompileShaders.cmake`
- [ ] Detectar `glslc` (Vulkan SDK)
- [ ] Adicionar função `compile_shader(SOURCE OUTPUT)`
- [ ] Target `compile_shaders` que compila todos os .vert/.frag
- [ ] Adicionar dependência: `Speed_Racer` depende de `compile_shaders`
- [ ] Testar: modificar shader → rebuild → .spv atualizado
- [ ] Gerar arquivo de manifesto (JSON) com bindings/locations 🆕

**Arquivos:**
```
cmake/CompileShaders.cmake
assets/shaders/CMakeLists.txt
assets/shaders/manifest.json (gerado)
```

**CMakeLists.txt:**
```cmake
include(cmake/CompileShaders.cmake)

file(GLOB_RECURSE SHADER_SOURCES 
    "assets/shaders/*.vert"
    "assets/shaders/*.frag"
)

foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    compile_shader(
        SOURCE ${SHADER}
        OUTPUT ${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}.spv
    )
endforeach()
```

---

**2. PipelineBuilder (COM REFLECTION BÁSICA)** 🔄

```cpp
class PipelineBuilder {
public:
    PipelineBuilder(VkDevice device, ResourceManager& resources);
    
    // Carrega automaticamente reflection data
    PipelineBuilder& addShader(VkShaderStageFlagBits stage, 
                               const std::string& path);
    
    // Configurações fluentes
    PipelineBuilder& setVertexInput(const VertexInputDescription& desc);
    PipelineBuilder& setTopology(VkPrimitiveTopology topology);
    PipelineBuilder& setViewport(VkExtent2D extent);
    PipelineBuilder& setRasterizer(VkPolygonMode mode, VkCullModeFlags cull);
    PipelineBuilder& setBlending(BlendMode mode);
    PipelineBuilder& setRenderPass(VkRenderPass renderPass);
    
    // Build retorna handle
    PipelineHandle build();
    
private:
    VkDevice device;
    ResourceManager& resources;
    ShaderReflection reflection;  // 🆕 Lê manifest.json
};
```

**Checklist:**
- [ ] Criar `PipelineBuilder.hpp/cpp`
- [ ] Parser de `manifest.json` → `ShaderReflection`
- [ ] Métodos fluentes (retornam `*this`)
- [ ] Validação de estado no `build()`
- [ ] Integração com ResourceManager (retorna PipelineHandle)
- [ ] Presets: `createBasicPipeline()`, `createWireframePipeline()`

**Métricas de Sucesso Dia 3:**
- ✅ Shaders compilam automaticamente no build
- ✅ PipelineBuilder cria pipeline com reflection
- ✅ Múltiplos pipelines facilmente criados
- ✅ Validação detecta configurações faltando

---

#### **DIA 4: Asset Management (SEM SINGLETON)** ⏱️ 6-8h

```cpp
class AssetManager {
public:
    AssetManager();
    
    void setBaseDirectory(const std::string& base);
    void setShaderDirectory(const std::string& dir);
    void setTextureDirectory(const std::string& dir);
    void setModelDirectory(const std::string& dir);
    
    std::string resolveShaderPath(const std::string& filename) const;
    std::string resolveTexturePath(const std::string& filename) const;
    std::string resolveModelPath(const std::string& filename) const;
    
    bool fileExists(const std::string& path) const;
    
private:
    std::filesystem::path basePath;
    std::filesystem::path shaderDir;
    std::filesystem::path textureDir;
    std::filesystem::path modelDir;
};
```

**Checklist:**
- [ ] Criar `AssetManager.hpp/cpp` (instância normal)
- [ ] Path resolution com `std::filesystem`
- [ ] Validação de existência de arquivos
- [ ] Logging de assets carregados
- [ ] Config file (YAML/JSON) para diretórios 🆕
- [ ] Testes: resolve paths, detecta arquivos faltando

**Config File (config.json):** 🆕
```json
{
  "assets": {
    "base": "assets",
    "shaders": "shaders/compiled",
    "textures": "textures",
    "models": "models"
  }
}
```

---

#### **DIA 5: Frames-in-Flight + Swapchain Validation** ⏱️ 8-10h 🆕

##### 📋 Tarefas (FORMALIZAÇÃO)

**Checklist:**
- [ ] Validar que `MAX_FRAMES_IN_FLIGHT = 2` funciona perfeitamente
- [ ] Adicionar teste: `MAX_FRAMES_IN_FLIGHT = 3` (deve funcionar)
- [ ] Testar swapchain recreation 10x seguidas (resize rápido)
- [ ] Testar minimização → restauração
- [ ] Adicionar timestamp queries (CPU/GPU time) 🆕
- [ ] Implementar debug markers (VkDebugUtilsLabel) 🆕
- [ ] Validar com RenderDoc (capturar frame)

**Arquivos:**
```
include/core/DebugUtils.hpp  🆕
src/core/DebugUtils.cpp
```

**Debug Markers:**
```cpp
void beginDebugLabel(VkCommandBuffer cmd, const char* name, glm::vec4 color);
void endDebugLabel(VkCommandBuffer cmd);

// Uso:
beginDebugLabel(cmd, "Draw Triangle", {1, 0, 0, 1});
vkCmdDraw(...);
endDebugLabel(cmd);
```

---

#### **DIA 6: Integração + Refatoração Final** ⏱️ 8-10h

**Checklist:**
- [ ] Refatorar VulkanManager para usar DI (não singletons)
- [ ] Passar referências explícitas entre managers
- [ ] Atualizar createGraphicsPipeline() com PipelineBuilder
- [ ] Remover todo código duplicado
- [ ] Adicionar sanitizers (ASan, UBSan) 🆕
- [ ] Configurar CI básico (GitHub Actions) 🆕
- [ ] Validar triângulo renderiza perfeitamente
- [ ] Documentação completa

**CI Config (.github/workflows/build.yml):** 🆕
```yaml
name: Build and Test
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install Vulkan SDK
        run: sudo apt-get install vulkan-sdk
      - name: Build
        run: |
          cmake -B build -DCMAKE_BUILD_TYPE=Debug
          cmake --build build
      - name: Run Tests
        run: ./build/Speed_Racer --test
```

**Métricas de Sucesso Semana 1:**
- ✅ VMA integrado
- ✅ DI ao invés de singletons
- ✅ Shaders compilam no CMake
- ✅ PipelineBuilder com reflection
- ✅ Frames-in-flight validado
- ✅ CI funcionando
- ✅ **Zero warnings, zero memory leaks**

---

```
╔══════════════════════════════════════════════════════════════╗
║    FASE 2: GEOMETRIA REAL + SCENE GRAPH (SEMANA 2)         ║
║                    🎨 4-5 DIAS                              ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 2: Real Geometry + Minimal Scene (REVISADO)

#### 🎯 Objetivo
Vertex/Index buffers + **Scene Graph mínimo** + **Mini FrameGraph**.

---

#### **DIA 1-2: Vertex + Index Buffers** ⏱️ 12-14h

**Checklist:**
- [ ] Criar struct `Vertex` (pos, color, normal, UV)
- [ ] `getBindingDescription()` e `getAttributeDescriptions()`
- [ ] Usar BufferManager para criar vertex buffer (VMA)
- [ ] Criar index buffer
- [ ] Atualizar PipelineBuilder com vertex input
- [ ] Modificar shaders para input attributes
- [ ] `vkCmdBindVertexBuffers` + `vkCmdBindIndexBuffer`
- [ ] `vkCmdDrawIndexed()` ao invés de `vkCmdDraw()`
- [ ] Testar cubo (8 vértices, 36 índices)

**Definição de Done:** 🆕
- ✅ Cubo renderizado com 8 vértices (não 36)
- ✅ Index buffer reduz memória vs vertex buffer puro
- ✅ Sem memory leaks (Valgrind)

---

#### **DIA 3: Scene Graph Mínimo + Transform** ⏱️ 8-10h 🆕

**Estrutura:**
```cpp
struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale = glm::vec3(1.0f);
    
    glm::mat4 getMatrix() const;
    glm::mat4 getWorldMatrix(const Transform* parent = nullptr) const;
};

struct Renderable {
    BufferHandle vertexBuffer;
    BufferHandle indexBuffer;
    uint32_t indexCount;
    PipelineHandle pipeline;
};

class SceneNode {
public:
    std::string name;
    Transform transform;
    Renderable* renderable = nullptr;  // nullptr se não renderiza
    
    std::vector<SceneNode*> children;
    SceneNode* parent = nullptr;
    
    void addChild(SceneNode* child);
    void removeChild(SceneNode* child);
    glm::mat4 getWorldTransform() const;
};

class Scene {
public:
    SceneNode* createNode(const std::string& name);
    void destroyNode(SceneNode* node);
    SceneNode* getRootNode() { return &root; }
    
    void traverse(std::function<void(SceneNode*)> visitor);
    
private:
    SceneNode root;
    std::vector<std::unique_ptr<SceneNode>> nodes;
};
```

**Checklist:**
- [ ] Criar `Transform.hpp/cpp`
- [ ] Criar `SceneNode.hpp/cpp`
- [ ] Criar `Scene.hpp/cpp`
- [ ] Implementar `traverse()` (depth-first)
- [ ] Refatorar render loop para iterar Scene
- [ ] Criar 2-3 nós com transforms diferentes
- [ ] Testar hierarquia pai/filho (cubo + filho rotacionando)

**Arquivos:**
```
include/scene/Transform.hpp
include/scene/SceneNode.hpp
include/scene/Scene.hpp
src/scene/Transform.cpp
src/scene/SceneNode.cpp
src/scene/Scene.cpp
```

---

#### **DIA 4: Mini FrameGraph** ⏱️ 8-10h 🆕

**Estrutura:**
```cpp
struct RenderPassInfo {
    std::string name;
    VkRenderPass renderPass;
    VkFramebuffer framebuffer;
    VkExtent2D extent;
    std::vector<VkClearValue> clearValues;
};

class FrameGraph {
public:
    using ExecuteFunc = std::function<void(VkCommandBuffer, const RenderPassInfo&)>;
    
    struct Pass {
        std::string name;
        RenderPassInfo passInfo;
        ExecuteFunc execute;
        std::vector<std::string> dependencies;
    };
    
    void addPass(const std::string& name, 
                 const RenderPassInfo& info,
                 ExecuteFunc func,
                 const std::vector<std::string>& deps = {});
    
    void compile();  // Ordena passes por dependências
    void execute(VkCommandBuffer cmd);
    
private:
    std::vector<Pass> passes;
    std::vector<Pass*> sortedPasses;  // Ordenado topologicamente
};
```

**Checklist:**
- [ ] Criar `FrameGraph.hpp/cpp`
- [ ] Implementar topological sort simples
- [ ] Adicionar 2 passes: "ForwardPass", "UIPass"
- [ ] Refatorar `drawFrame()` para usar FrameGraph
- [ ] Validar ordem de execução
- [ ] Adicionar debug labels por pass

**Exemplo de Uso:**
```cpp
frameGraph.addPass("Forward", forwardPassInfo, 
    [this](VkCommandBuffer cmd, const RenderPassInfo& info) {
        scene.traverse([&](SceneNode* node) {
            if (node->renderable) {
                // Bind pipeline, buffers, draw
            }
        });
    });

frameGraph.addPass("UI", uiPassInfo,
    [this](VkCommandBuffer cmd, const RenderPassInfo& info) {
        // Draw UI
    }, 
    {"Forward"});  // Depende de Forward

frameGraph.compile();
frameGraph.execute(commandBuffer);
```

**Definição de Done:** 🆕
- ✅ 2 passes executam em ordem correta
- ✅ Debug markers visíveis em RenderDoc
- ✅ Fácil adicionar novos passes

---

#### **DIA 5: Advanced Geometry + Profiling** ⏱️ 8h

**Checklist:**
- [ ] Criar classe `Mesh` (vertices + indices encapsulados)
- [ ] Geometria procedural: `createCube()`, `createSphere()`, `createPlane()`
- [ ] Adicionar 10+ objetos na cena
- [ ] Timestamp queries (GPU time por pass) 🆕
- [ ] Performance profiling (RenderDoc/Tracy)
- [ ] Otimizar: batch staging uploads

**Definição de Done:** 🆕
- ✅ 50+ objetos renderizados a 60+ FPS (1080p)
- ✅ GPU time < 10ms por frame
- ✅ Documentação de `Mesh` e `Scene`

---

```
╔══════════════════════════════════════════════════════════════╗
║       FASE 3: UNIFORMS + TEXTURES + CAMERA (SEMANA 3)      ║
║                    🌐 5-6 DIAS                              ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 3: Descriptors, Textures, Camera (MANTIDO COM AJUSTES)

#### **DIA 1-2: Descriptor System + UBO** ⏱️ 12-14h

**DescriptorManager (COM INJEÇÃO):**
```cpp
class DescriptorManager {
public:
    DescriptorManager(VkDevice device, ResourceManager& resources);
    
    DescriptorPoolHandle createPool(const PoolSizes& sizes, uint32_t maxSets);
    DescriptorSetLayoutHandle createLayout(const std::vector<Binding>& bindings);
    
    std::vector<VkDescriptorSet> allocateSets(
        DescriptorPoolHandle pool,
        DescriptorSetLayoutHandle layout,
        uint32_t count);
    
    void updateSet(VkDescriptorSet set, const std::vector<WriteDescriptor>& writes);
    
private:
    VkDevice device;
    ResourceManager& resources;
};
```

**Checklist:**
- [ ] Criar `DescriptorManager.hpp/cpp`
- [ ] Struct `UniformBufferObject` (M, V, P matrices)
- [ ] Criar UBO por frame in flight (BufferManager)
- [ ] Descriptor pool + layout
- [ ] Allocate descriptor sets (um por frame)
- [ ] Update descriptor sets apontando para UBOs
- [ ] Atualizar PipelineBuilder com descriptor layout
- [ ] Shader com UBO: `layout(set=0, binding=0) uniform UBO { ... }`
- [ ] Implementar rotação simples (modelo)
- [ ] Testar transformações

**Definição de Done:** 🆕
- ✅ Cubo rotacionando suavemente
- ✅ Múltiplos objetos com transforms independentes
- ✅ Sem flickering ou race conditions

---

#### **DIA 3-4: Texture System** ⏱️ 14-16h

**TextureManager:**
```cpp
class TextureManager {
public:
    TextureManager(VkDevice device, 
                   VkPhysicalDevice physDevice,
                   VmaAllocator allocator,
                   ResourceManager& resources,
                   CommandManager& commands);
    
    ImageHandle loadTexture(const std::string& path);
    VkSampler createSampler(const SamplerCreateInfo& info);
    
private:
    void createImage(const ImageCreateInfo& info);
    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};
```

**Checklist:**
- [ ] Integrar stb_image
- [ ] `loadTexture()` end-to-end (load → staging → device image → sampler)
- [ ] Image layout transitions (barriers)
- [ ] Mipmaps (opcional, mas recomendado)
- [ ] Adicionar UV ao `Vertex`
- [ ] Atualizar descriptor set (binding=1: sampler2D)
- [ ] Shader textured: `texture(texSampler, fragTexCoord)`
- [ ] Testar com textura checkerboard

**Definição de Done:** 🆕
- ✅ Textura aplicada corretamente (sem distorção)
- ✅ Mipmaps funcionando (LOD visível ao afastar)
- ✅ 5+ texturas diferentes carregadas

---

#### **DIA 5-6: Camera System** ⏱️ 10-12h

**Camera:**
```cpp
class Camera {
public:
    void setPosition(const glm::vec3& pos);
    void setRotation(float pitch, float yaw);
    void setPerspective(float fov, float aspect, float near, float far);
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    
    void processInput(GLFWwindow* window, float deltaTime);
    void processMouseMove(double xpos, double ypos);
    
private:
    glm::vec3 position{0, 0, 3};
    float pitch = 0.0f;
    float yaw = -90.0f;
    float fov = 45.0f;
};
```

**Checklist:**
- [ ] Criar `Camera.hpp/cpp`
- [ ] `lookAt()` para view matrix
- [ ] `perspective()` para projection
- [ ] Input processing (WASD + mouse)
- [ ] Atualizar UBO com camera matrices
- [ ] Smooth movement (delta time)
- [ ] Mouse capture (glfwSetInputMode)
- [ ] Testar FPS camera

**Definição de Done:** 🆕
- ✅ Câmera FPS fluida (sem judder a 60+ FPS)
- ✅ 6 DOF completo (WASD + mouse look)
- ✅ Não trava ao mover rápido

---

```
╔══════════════════════════════════════════════════════════════╗
║         FASE 4: DEPTH + MODEL LOADING (SEMANA 4)            ║
║                    🗿 5-6 DIAS                              ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 4: Depth + Models (MANTIDO)

#### **DIA 1-2: Depth Buffering** ⏱️ 12h

**Checklist:**
- [ ] Encontrar formato depth (VK_FORMAT_D32_SFLOAT)
- [ ] Criar depth image (VMA)
- [ ] Criar depth image view
- [ ] Atualizar RenderPass (2 attachments: color + depth)
- [ ] Configurar depth testing no pipeline
- [ ] Atualizar framebuffers
- [ ] Clear depth (1.0)
- [ ] Testar geometria sobreposta

**Definição de Done:** 🆕
- ✅ Cubos sobrepostos renderizam corretamente
- ✅ Z-fighting não aparece em objetos coincidentes
- ✅ Depth buffer visível em RenderDoc

---

#### **DIA 3-5: Model Loading** ⏱️ 20-24h

**ModelLoader:**
```cpp
class ModelLoader {
public:
    ModelLoader(BufferManager& buffers, TextureManager& textures, AssetManager& assets);
    
    std::unique_ptr<Model> loadModel(const std::string& path);
    
private:
    struct LoadedMesh {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        Material material;
    };
    
    LoadedMesh processNode(aiNode* node, const aiScene* scene);
};
class Model {
public:
    std::vector<Mesh> meshes;
    std::vector<Material> materials;
    std::string name;
    
    void draw(VkCommandBuffer cmd, VkPipelineLayout layout);
    BoundingBox getBounds() const;
};

struct Material {
    glm::vec3 diffuseColor = glm::vec3(1.0f);
    glm::vec3 specularColor = glm::vec3(1.0f);
    float shininess = 32.0f;
    
    ImageHandle diffuseTexture;
    ImageHandle specularTexture;
    ImageHandle normalMap;
};
```

**Checklist:**
- [ ] Integrar Assimp (já presente no CMakeLists)
- [ ] Criar `ModelLoader.hpp/cpp`
- [ ] Implementar `loadOBJ()` → retorna `Model`
- [ ] Processar meshes (vertices, indices, materials)
- [ ] Carregar texturas associadas (diffuse, specular)
- [ ] Otimizar: compartilhar vértices duplicados
- [ ] Criar `MeshManager` para cache de modelos
- [ ] Testar com modelo simples (suzanne.obj)
- [ ] Testar com modelo complexo (car.obj, 100k+ polys)
- [ ] Adicionar bounding boxes (culling futuro)

**Arquivos:**
```
include/rendering/ModelLoader.hpp
src/rendering/ModelLoader.cpp
include/rendering/Model.hpp
include/rendering/Material.hpp
include/rendering/Mesh.hpp
src/rendering/Model.cpp
```

**Definição de Done:** 🆕
- ✅ Modelo complexo (50k+ polígonos) renderiza corretamente
- ✅ Múltiplos materiais aplicados
- ✅ Texturas carregadas automaticamente
- ✅ Performance: 60 FPS com 5+ modelos complexos
- ✅ Memory usage razoável (< 500MB para 10 modelos)

---

#### **DIA 6: Polish, Profiling & CI** ⏱️ 8-10h

**Checklist:**
- [ ] Profiling detalhado (CPU/GPU time por subsistema)
- [ ] Identificar gargalos (VK_QUERY_TYPE_TIMESTAMP)
- [ ] Otimizar uploads (batch buffers)
- [ ] Pipeline cache persistido em disco 🆕
- [ ] Validar memory usage (Valgrind/ASan)
- [ ] CI: smoke test com modelo + textura 🆕
- [ ] Documentação completa da arquitetura
- [ ] Update README com screenshots

**Pipeline Cache:** 🆕
```cpp
// Salvar cache
VkPipelineCache cache;
vkCreatePipelineCache(device, &info, nullptr, &cache);
// ... criar pipelines com cache ...
size_t dataSize;
vkGetPipelineCacheData(device, cache, &dataSize, nullptr);
std::vector<uint8_t> data(dataSize);
vkGetPipelineCacheData(device, cache, &dataSize, data.data());
// Salvar data em arquivo "pipeline.cache"

// Carregar cache
std::vector<uint8_t> cachedData = loadFile("pipeline.cache");
VkPipelineCacheCreateInfo cacheInfo{};
cacheInfo.initialDataSize = cachedData.size();
cacheInfo.pInitialData = cachedData.data();
vkCreatePipelineCache(device, &cacheInfo, nullptr, &cache);
```

**Definição de Done (Semana 4):** 🆕
- ✅ Engine renderiza cenas 3D complexas (10+ modelos, 50+ objetos)
- ✅ Performance: 60+ FPS a 1080p
- ✅ Memory leaks: ZERO (Valgrind clean)
- ✅ CI passa todos os testes
- ✅ Documentação completa e atualizada

---

```
╔══════════════════════════════════════════════════════════════╗
║      FASE 5: ILUMINAÇÃO E MATERIAIS (SEMANA 5-6)            ║
║                    💡 8-10 DIAS                             ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 5-6: Lighting & Materials

#### 🎯 Objetivo
Sistema de iluminação robusto (Phong/Blinn-Phong) + materiais avançados.

---

#### **DIA 1-3: Lighting System** ⏱️ 18-20h

**Light Types:**
```cpp
enum class LightType {
    DIRECTIONAL,
    POINT,
    SPOT
};

struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
};

struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float radius;
    
    // Attenuation
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float innerCutoff;  // cos(angle)
    float outerCutoff;
};

struct LightingUBO {
    DirectionalLight dirLight;
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    int numPointLights;
    int numSpotLights;
    glm::vec3 viewPos;  // Camera position
};
```

**Checklist:**
- [ ] Adicionar `normal` ao `Vertex` struct
- [ ] Atualizar `ModelLoader` para carregar normais
- [ ] Criar `LightManager.hpp/cpp`
- [ ] Implementar UBO de iluminação
- [ ] Shader de iluminação (Blinn-Phong)
  - [ ] Ambient component
  - [ ] Diffuse component (N·L)
  - [ ] Specular component (N·H)
- [ ] Adicionar múltiplas luzes à cena
- [ ] Debug visualization (esferas para point lights)
- [ ] Testar com diferentes configurações

**Shader (Fragment):**
```glsl
#version 450

layout(set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(set = 0, binding = 1) uniform LightingUBO {
    DirectionalLight dirLight;
    PointLight pointLights[4];
    int numPointLights;
    vec3 viewPos;
} lights;

layout(set = 0, binding = 2) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(lights.viewPos - fragPos);
    
    // Directional light
    vec3 result = calcDirectionalLight(lights.dirLight, norm, viewDir);
    
    // Point lights
    for(int i = 0; i < lights.numPointLights; i++) {
        result += calcPointLight(lights.pointLights[i], norm, fragPos, viewDir);
    }
    
    vec3 texColor = texture(texSampler, fragTexCoord).rgb;
    outColor = vec4(result * texColor, 1.0);
}
```

**Arquivos:**
```
include/rendering/LightManager.hpp
src/rendering/LightManager.cpp
include/rendering/Light.hpp
assets/shaders/lighting/phong.vert
assets/shaders/lighting/phong.frag
```

**Definição de Done:** 🆕
- ✅ 3+ tipos de luzes funcionando simultaneamente
- ✅ Iluminação realista (Blinn-Phong)
- ✅ Debug visualization de luzes
- ✅ Performance: 60 FPS com 10+ luzes

---

#### **DIA 4-7: Advanced Materials** ⏱️ 24-28h

**Material System:**
```cpp
enum class MaterialType {
    PHONG,
    PBR,           // Futuro
    UNLIT
};

struct PhongMaterial {
    glm::vec3 ambient = glm::vec3(0.1f);
    glm::vec3 diffuse = glm::vec3(0.8f);
    glm::vec3 specular = glm::vec3(1.0f);
    float shininess = 32.0f;
    
    ImageHandle diffuseMap;
    ImageHandle specularMap;
    ImageHandle normalMap;
    ImageHandle emissiveMap;
};

class MaterialManager {
public:
    MaterialManager(ResourceManager& resources, TextureManager& textures);
    
    MaterialHandle createMaterial(const PhongMaterial& mat);
    void updateMaterial(MaterialHandle handle, const PhongMaterial& mat);
    
    VkDescriptorSet getDescriptorSet(MaterialHandle handle) const;
    
private:
    std::unordered_map<MaterialHandle, PhongMaterial> materials;
    std::unordered_map<MaterialHandle, VkDescriptorSet> descriptorSets;
};
```

**Checklist:**
- [ ] Criar `MaterialManager.hpp/cpp`
- [ ] Struct `PhongMaterial` completa
- [ ] Integrar com `ModelLoader` (ler .mtl files)
- [ ] Descriptor set por material (textures + properties)
- [ ] Push constants para material properties 🆕
- [ ] Shader variants (com/sem normal map, etc.)
- [ ] Material editor (ImGui) 🆕
- [ ] Hot-reload de materiais 🆕
- [ ] Testar com 10+ materiais diferentes

**Push Constants:** 🆕
```cpp
struct MaterialPushConstants {
    glm::vec3 diffuseColor;
    float shininess;
    int hasNormalMap;
    int hasSpecularMap;
};

vkCmdPushConstants(cmd, pipelineLayout, 
                   VK_SHADER_STAGE_FRAGMENT_BIT,
                   0, sizeof(MaterialPushConstants), &pushData);
```

**Arquivos:**
```
include/rendering/MaterialManager.hpp
src/rendering/MaterialManager.cpp
include/rendering/PhongMaterial.hpp
assets/shaders/materials/phong_textured.frag
assets/shaders/materials/phong_normal_mapped.frag
```

**Definição de Done:** 🆕
- ✅ 10+ materiais únicos renderizando
- ✅ Normal mapping funcional
- ✅ Specular maps aplicados corretamente
- ✅ Hot-reload atualiza materiais em real-time
- ✅ Material editor (ImGui) funcional

---

#### **DIA 8-10: Integration & Polish** ⏱️ 16-20h

**Checklist:**
- [ ] Integrar iluminação com scene graph
- [ ] Light components no scene graph
- [ ] Material assignment por mesh
- [ ] Frustum culling básico 🆕
- [ ] Occlusion culling (preparação) 🆕
- [ ] Performance profiling detalhado
- [ ] Memory optimizations
- [ ] Documentação completa

**Frustum Culling:** 🆕
```cpp
class Frustum {
public:
    void update(const glm::mat4& viewProj);
    bool intersects(const BoundingBox& bbox) const;
    
private:
    std::array<glm::vec4, 6> planes;  // Left, Right, Top, Bottom, Near, Far
};

// Uso no render loop:
scene.traverse([&](SceneNode* node) {
    if (node->renderable) {
        BoundingBox worldBBox = node->renderable->bounds.transform(node->getWorldTransform());
        if (frustum.intersects(worldBBox)) {
            // Draw
        }
    }
});
```

**Definição de Done (Semana 5-6):** 🆕
- ✅ Cena realista com iluminação e materiais
- ✅ 100+ objetos com frustum culling: 60+ FPS
- ✅ Múltiplos materiais e luzes
- ✅ Hot-reload funcional
- ✅ Documentação e screenshots atualizados

---

```
╔══════════════════════════════════════════════════════════════╗
║       FASE 6: SCENE & INTERACTIVITY (SEMANA 7-8)            ║
║                    🌳 8-10 DIAS                             ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 7-8: Advanced Scene & Interaction

#### 🎯 Objetivo
ECS/Component system robusto + Input abstrato + Game loop.

---

#### **DIA 1-4: Component System** ⏱️ 24-28h

**ECS Lite (ou Full ECS):**
```cpp
// Opção 1: ECS Lite (GameObject-based)
class Component {
public:
    virtual ~Component() = default;
    virtual void update(float deltaTime) {}
};

class GameObject {
public:
    std::string name;
    Transform transform;
    
    template<typename T>
    T* addComponent();
    
    template<typename T>
    T* getComponent();
    
    void update(float deltaTime);
    
private:
    std::vector<std::unique_ptr<Component>> components;
};

// Componentes específicos
class RenderComponent : public Component {
public:
    Model* model;
    Material* material;
};

class LightComponent : public Component {
public:
    PointLight light;
};

class PhysicsComponent : public Component {
public:
    glm::vec3 velocity;
    float mass;
    void update(float deltaTime) override;
};
```

**OU Opção 2: Full ECS (EnTT recommended)** 🆕
```cpp
#include <entt/entt.hpp>

// Components são structs simples
struct TransformComponent {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

struct RenderComponent {
    Model* model;
    Material* material;
};

struct VelocityComponent {
    glm::vec3 velocity;
};

// Systems
class RenderSystem {
public:
    void update(entt::registry& registry, VkCommandBuffer cmd) {
        auto view = registry.view<TransformComponent, RenderComponent>();
        for(auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& render = view.get<RenderComponent>(entity);
            // Draw
        }
    }
};

class PhysicsSystem {
public:
    void update(entt::registry& registry, float dt) {
        auto view = registry.view<TransformComponent, VelocityComponent>();
        for(auto entity : view) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& velocity = view.get<VelocityComponent>(entity);
            transform.position += velocity.velocity * dt;
        }
    }
};
```

**Checklist:**
- [ ] Decidir: ECS Lite vs Full ECS (EnTT)
- [ ] Implementar sistema escolhido
- [ ] Criar componentes core:
  - [ ] `TransformComponent`
  - [ ] `RenderComponent`
  - [ ] `LightComponent`
  - [ ] `VelocityComponent`
  - [ ] `CameraComponent`
- [ ] Implementar systems:
  - [ ] `RenderSystem`
  - [ ] `PhysicsSystem` (básico)
  - [ ] `LightingSystem`
- [ ] Refatorar Scene para usar ECS
- [ ] Serialização básica (save/load scene) 🆕
- [ ] Testes: criar/destruir 1000 entities

**Arquivos:**
```
include/ecs/Component.hpp
include/ecs/GameObject.hpp (se ECS Lite)
include/ecs/Registry.hpp (se Full ECS)
include/ecs/Systems.hpp
src/ecs/*.cpp
```

**Serialização (JSON):** 🆕
```json
{
  "scene": "MainScene",
  "entities": [
    {
      "name": "Player",
      "components": {
        "transform": {
          "position": [0, 0, 0],
          "rotation": [0, 0, 0, 1],
          "scale": [1, 1, 1]
        },
        "render": {
          "model": "models/player.obj",
          "material": "materials/player.json"
        }
      }
    }
  ]
}
```

**Definição de Done:** 🆕
- ✅ 1000+ entities criadas/destruídas sem leaks
- ✅ ECS system funcional e performático
- ✅ Scene serialization funciona
- ✅ Componentes podem ser adicionados/removidos em runtime

---

#### **DIA 5-6: Input System** ⏱️ 12-14h

**Input Manager:**
```cpp
enum class InputAction {
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP,
    SHOOT,
    INTERACT
};

class InputManager {
public:
    void initialize(GLFWwindow* window);
    
    void bindAction(InputAction action, int key);
    void bindAxis(const std::string& axisName, int positiveKey, int negativeKey);
    
    bool isActionPressed(InputAction action) const;
    bool isActionJustPressed(InputAction action) const;
    float getAxis(const std::string& axisName) const;
    
    glm::vec2 getMouseDelta() const;
    glm::vec2 getMousePosition() const;
    
    void update();  // Call every frame
    
private:
    GLFWwindow* window;
    std::unordered_map<InputAction, int> actionBindings;
    std::unordered_map<std::string, std::pair<int, int>> axisBindings;
    
    std::unordered_set<int> currentKeys;
    std::unordered_set<int> previousKeys;
    
    glm::vec2 mousePosition;
    glm::vec2 lastMousePosition;
};
```

**Checklist:**
- [ ] Criar `InputManager.hpp/cpp`
- [ ] Abstrair callbacks GLFW
- [ ] Action mapping (key → action)
- [ ] Axis mapping (keys → float [-1, 1])
- [ ] Mouse delta calculation
- [ ] Config file para bindings 🆕
- [ ] Gamepad support (preparação) 🆕
- [ ] Input recording/playback (debug) 🆕

**Config (input.json):** 🆕
```json
{
  "actions": {
    "MOVE_FORWARD": "W",
    "MOVE_BACKWARD": "S",
    "MOVE_LEFT": "A",
    "MOVE_RIGHT": "D",
    "JUMP": "SPACE",
    "SHOOT": "MOUSE_LEFT"
  },
  "axes": {
    "Horizontal": {"positive": "D", "negative": "A"},
    "Vertical": {"positive": "W", "negative": "S"}
  }
}
```

---

#### **DIA 7-8: Player Controller & Game Loop** ⏱️ 14-16h

**Player Controller Component:**
```cpp
class PlayerControllerComponent : public Component {
public:
    float moveSpeed = 5.0f;
    float sprintMultiplier = 2.0f;
    float jumpForce = 10.0f;
    
    void update(float deltaTime) override;
    
private:
    InputManager& input;
    Camera& camera;
};

// Uso:
void PlayerControllerComponent::update(float dt) {
    glm::vec3 move(0);
    
    if (input.isActionPressed(InputAction::MOVE_FORWARD))
        move += camera.getForward();
    if (input.isActionPressed(InputAction::MOVE_BACKWARD))
        move -= camera.getForward();
    // ...
    
    float speed = moveSpeed;
    if (input.isActionPressed(InputAction::SPRINT))
        speed *= sprintMultiplier;
    
    gameObject->transform.position += move * speed * dt;
}
```

**Game Loop:**
```cpp
class Game {
public:
    void run() {
        float lastTime = glfwGetTime();
        
        while (!window.shouldClose()) {
            float currentTime = glfwGetTime();
            float deltaTime = currentTime - lastTime;
            lastTime = currentTime;
            
            // Fixed timestep physics
            accumulator += deltaTime;
            while (accumulator >= fixedDeltaTime) {
                fixedUpdate(fixedDeltaTime);
                accumulator -= fixedDeltaTime;
            }
            
            // Variable timestep rendering
            update(deltaTime);
            render();
            
            window.pollEvents();
        }
    }
    
private:
    void fixedUpdate(float dt);  // Physics, gameplay logic
    void update(float dt);        // Input, animations, camera
    void render();                // Rendering
    
    float fixedDeltaTime = 1.0f / 60.0f;
    float accumulator = 0.0f;
};
```

**Checklist:**
- [ ] Implementar `PlayerControllerComponent`
- [ ] Fixed timestep para física
- [ ] Variable timestep para rendering
- [ ] Frame rate independence
- [ ] Delta time smoothing 🆕
- [ ] Pause system 🆕
- [ ] Time scale (bullet time) 🆕

**Definição de Done:** 🆕
- ✅ Player controller responsivo (< 50ms input lag)
- ✅ Frame rate independent (30 FPS vs 144 FPS mesmo comportamento)
- ✅ Input config funcionando
- ✅ Pause system funcional

---

#### **DIA 9-10: Integration & Testing** ⏱️ 12-16h

**Checklist:**
- [ ] Integrar todos os sistemas
- [ ] Criar cena de teste completa
- [ ] Performance profiling (1000+ entities)
- [ ] Memory profiling
- [ ] Stress testing (spawn 10k entities)
- [ ] Save/load scene
- [ ] Documentação completa
- [ ] Video demo 🆕

**Definição de Done (Semana 7-8):** 🆕
- ✅ Engine completa com ECS, input, e game loop
- ✅ Scene complexa (100+ entities) a 60+ FPS
- ✅ Player controller funcional
- ✅ Save/load funciona perfeitamente
- ✅ Documentação e video demo prontos

---

```
╔══════════════════════════════════════════════════════════════╗
║         FASE 7: ADVANCED RENDERING (SEMANA 9-12)            ║
║                    ✨ 15-20 DIAS                             ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 SEMANA 9-12: Advanced Graphics

#### 🎯 Objetivos
Shadow mapping, skybox, post-processing, instancing, PBR básico.

---

#### **SEMANA 9: Shadows & Skybox** ⏱️ 5 dias

**Shadow Mapping:**
```cpp
class ShadowMapper {
public:
    void initialize(uint32_t resolution = 2048);
    void renderShadowMap(const DirectionalLight& light, const Scene& scene);
    VkImageView getShadowMapView() const;
    glm::mat4 getLightSpaceMatrix() const;
};
```

**Checklist:**
- [ ] Criar depth-only render pass
- [ ] Shadow map framebuffer (2048x2048)
- [ ] Light space matrix calculation
- [ ] Shadow pass no FrameGraph
- [ ] PCF filtering (soft shadows)
- [ ] Shadow acne fix (bias)
- [ ] Cascaded shadow maps (CSM) 🆕
- [ ] Skybox com cubemap

**Definição de Done:** 🆕
- ✅ Sombras suaves e realistas
- ✅ Sem shadow acne ou peter panning
- ✅ CSM para distâncias grandes
- ✅ Skybox renderizado corretamente

---

#### **SEMANA 10: Post-Processing** ⏱️ 5 dias

**Post-Process Stack:**
```cpp
class PostProcessStack {
public:
    void addEffect(std::unique_ptr<PostProcessEffect> effect);
    void render(VkCommandBuffer cmd, VkImageView input, VkImageView output);
    
private:
    std::vector<std::unique_ptr<PostProcessEffect>> effects;
};

// Effects:
// - Bloom
// - Tone mapping (ACES, Reinhard)
// - FXAA/SMAA
// - Color grading
// - Vignette
// - Chromatic aberration
```

**Checklist:**
- [ ] Offscreen framebuffer (HDR)
- [ ] Bloom (gaussian blur)
- [ ] Tone mapping
- [ ] FXAA
- [ ] Color grading (LUT)
- [ ] Vignette
- [ ] Compositing final

**Definição de Done:** 🆕
- ✅ 5+ post-process effects funcionando
- ✅ Minimal performance impact (< 5ms)
- ✅ Toggle effects em runtime

---

#### **SEMANA 11-12: PBR & Instancing** ⏱️ 8-10 dias

**PBR Materials:**
```cpp
struct PBRMaterial {
    glm::vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    
    ImageHandle albedoMap;
    ImageHandle metallicRoughnessMap;
    ImageHandle normalMap;
    ImageHandle aoMap;
    ImageHandle emissiveMap;
};
```

**Instanced Rendering:**
```cpp
// Draw 10k identical objects em uma draw call
struct InstanceData {
    glm::mat4 model;
    glm::vec4 color;
};

std::vector<InstanceData> instances(10000);
BufferHandle instanceBuffer = bufferMgr.createBuffer(instances);

vkCmdDrawIndexed(cmd, indexCount, 10000, 0, 0, 0);
```

**Checklist:**
- [ ] PBR shader (Cook-Torrance BRDF)
- [ ] IBL (Image-Based Lighting)
- [ ] Instanced rendering
- [ ] Indirect drawing
- [ ] GPU frustum culling 🆕
- [ ] LOD system 🆕

**Definição de Done:** 🆕
- ✅ PBR materials realistas
- ✅ 10k+ instances a 60 FPS
- ✅ GPU culling funcional
- ✅ LOD transitions suaves

---

```
╔══════════════════════════════════════════════════════════════╗
║           FASE 8: GAME ENGINE FEATURES (FUTURO)             ║
║                    🚀 LONGO PRAZO                             ║
╚══════════════════════════════════════════════════════════════╝
```

### 📅 Futuro: Full Game Engine

**Physics:**
- [ ] Integrar Bullet/PhysX/Jolt
- [ ] Rigid body dynamics
- [ ] Collision detection
- [ ] Character controller

**Animation:**
- [ ] Skeletal animation
- [ ] Blend trees
- [ ] IK (Inverse Kinematics)

**Audio:**
- [ ] OpenAL/FMOD
- [ ] 3D spatial audio
- [ ] Music system

**UI:**
- [ ] ImGui integration (já presente)
- [ ] In-game UI renderer
- [ ] HUD system

**Scripting:**
- [ ] Lua/AngelScript
- [ ] Hot-reload scripts
- [ ] Sandboxed execution

**Editor:**
- [ ] Scene editor
- [ ] Material editor
- [ ] Inspector
- [ ] Asset browser

**Networking:**
- [ ] Client-server architecture
- [ ] Replication
- [ ] Lag compensation

---

## 🎯 CRITÉRIOS DE ACEITE GLOBAIS

### Semana 1 (Fundação)
- ✅ Zero singletons (DI everywhere)
- ✅ VMA integrado
- ✅ Shader toolchain no CMake
- ✅ CI funcional
- ✅ Zero memory leaks

### Semana 2 (Geometria + Scene)
- ✅ Vertex/Index buffers
- ✅ Scene graph básico
- ✅ Mini FrameGraph
- ✅ 50+ objetos a 60 FPS

### Semana 3 (Descriptors + Textures)
- ✅ UBO MVP funcional
- ✅ Texturas carregadas
- ✅ Câmera FPS fluida

### Semana 4 (Depth + Models)
- ✅ Depth buffer correto
- ✅ Modelos complexos carregados
- ✅ 60 FPS com 10+ modelos

### Semana 5-6 (Lighting)
- ✅ Iluminação realista
- ✅ 10+ materiais
- ✅ Hot-reload funcional
- ✅ Frustum culling

### Semana 7-8 (ECS + Input)
- ✅ ECS system robusto
- ✅ Input abstrato
- ✅ Player controller
- ✅ Save/load scene

### Semana 9-12 (Advanced)
- ✅ Shadows + Skybox
- ✅ Post-processing
- ✅ PBR + Instancing
- ✅ 10k+ instances a 60 FPS

---

## 📊 TRACKING DE PROGRESSO

```
╔═══════════════════════════════════════════════════════════╗
║  SEMANA  │  FEATURE              │  STATUS  │  BLOCKERS  ║
╠═══════════════════════════════════════════════════════════╣
║    1     │  VMA + DI             │  [ ]     │            ║
║    1     │  Shader Toolchain     │  [ ]     │            ║
║    1     │  PipelineBuilder      │  [ ]     │            ║
║    2     │  Vertex/Index         │  [ ]     │            ║
║    2     │  Scene Graph          │  [ ]     │            ║
║    3     │  UBO + Camera         │  [ ]     │            ║
║    3     │Textures             │  [ ]     │            ║
║    4     │  Depth Buffer         │  [ ]     │            ║
║    4     │  Model Loading        │  [ ]     │            ║
║   5-6    │  Lighting System      │  [ ]     │            ║
║   5-6    │  Materials            │  [ ]     │            ║
║   7-8    │  ECS/Components       │  [ ]     │            ║
║   7-8    │  Input + Controller   │  [ ]     │            ║
║   9-12   │  Advanced Rendering   │  [ ]     │            ║
╚═══════════════════════════════════════════════════════════╝
```

---

## 🔧 FERRAMENTAS RECOMENDADAS

### Development
- **IDE:** CLion, VSCode com clangd
- **Build:** CMake 3.22+, Ninja
- **Compiler:** GCC 11+, Clang 14+
- **Sanitizers:** ASan, UBSan, TSan

### Debugging & Profiling
- **Graphics:** RenderDoc, NSight Graphics
- **Performance:** Tracy Profiler, Superluminal
- **Memory:** Valgrind, Heaptrack
- **GPU:** Vulkan Validation Layers (sempre ativo em Debug)

### Assets
- **3D Models:** Blender, Maya
- **Textures:** Substance Designer/Painter, Photoshop
- **Audio:** Audacity, FMOD Studio

### Version Control
- **Git:** Conventional Commits, GitFlow
- **LFS:** Para assets binários grandes

---

## 📚 RECURSOS DE APRENDIZADO

### Vulkan
- [Vulkan Tutorial](https://vulkan-tutorial.com/) - Base fundamental
- [Vulkan Guide](https://vkguide.dev/) - Práticas modernas
- [Sascha Willems Examples](https://github.com/SaschaWillems/Vulkan) - Referência

### Engine Architecture
- [Game Engine Architecture (Book)](https://www.gameenginebook.com/)
- [Real-Time Rendering](https://www.realtimerendering.com/)
- [GPU Gems](https://developer.nvidia.com/gpugems/)

### Graphics Programming
- [Learn OpenGL](https://learnopengl.com/) - Conceitos transferíveis
- [Physically Based Rendering Book](https://pbr-book.org/)
- [Ray Tracing in One Weekend](https://raytracing.github.io/)

### Best Practices
- [Vulkan Best Practices (ARM)](https://arm-software.github.io/vulkan_best_practice_for_mobile_developers/)
- [GPU Performance Optimization (NVIDIA)](https://developer.nvidia.com/blog/vulkan-dos-donts/)

---

## 🚨 ARMADILHAS COMUNS E COMO EVITAR

### 1. **Memory Leaks**
```cpp
// ❌ MAL:
VkBuffer buffer;
vkCreateBuffer(device, &info, nullptr, &buffer);
// Esqueceu de destruir

// ✅ BOM:
struct ScopedBuffer {
    VkBuffer buffer;
    VkDevice device;
    ~ScopedBuffer() { vkDestroyBuffer(device, buffer, nullptr); }
};
```

### 2. **Synchronization Bugs**
```cpp
// ❌ MAL:
vkAcquireNextImageKHR(...);
// Usa imageIndex imediatamente sem fence

// ✅ BOM:
vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
vkAcquireNextImageKHR(..., semaphore, ...);
// Agora é seguro usar
```

### 3. **Pipeline Stalls**
```cpp
// ❌ MAL:
for (auto& obj : objects) {
    vkCmdBindPipeline(..., obj.pipeline);  // Pipeline switch por objeto
    vkCmdDraw(...);
}

// ✅ BOM:
// Agrupe por pipeline
for (auto& [pipeline, objects] : groupedByPipeline) {
    vkCmdBindPipeline(..., pipeline);
    for (auto& obj : objects) {
        vkCmdDraw(...);
    }
}
```

### 4. **Descriptor Set Management**
```cpp
// ❌ MAL:
// Criar descriptor set por draw call (lento!)

// ✅ BOM:
// Pool de descriptor sets reutilizáveis
// ou descriptor indexing (Vulkan 1.2+)
```

### 5. **Hardcoded Paths**
```cpp
// ❌ MAL:
loadTexture("C:/Users/Me/Project/assets/texture.png");

// ✅ BOM:
assetManager.resolveTexturePath("texture.png");
```

### 6. **Singleton Hell**
```cpp
// ❌ MAL:
ResourceManager::getInstance().doSomething();
TextureManager::getInstance().doSomething();
// Acoplamento oculto, difícil testar

// ✅ BOM:
class Renderer {
    ResourceManager& resources;
    TextureManager& textures;
public:
    Renderer(ResourceManager& r, TextureManager& t) 
        : resources(r), textures(t) {}
};
```

### 7. **Premature Optimization**
```cpp
// ❌ MAL:
// Otimizar antes de medir
// "Esse código DEVE ser lento"

// ✅ BOM:
// 1. Fazer funcionar
// 2. Medir com profiler
// 3. Otimizar gargalos reais
```

---

## 🎯 MILESTONES VISUAIS

### Milestone 1: "Hello Triangle" (Semana 1) ✅
![Triangle](docs/images/milestone1_triangle.png)
- Triângulo colorido renderizado
- Window resize funciona
- Validation layers ativas

### Milestone 2: "Real Geometry" (Semana 2)
![Geometry](docs/images/milestone2_geometry.png)
- Múltiplos cubos
- Vertex/Index buffers
- Scene graph básico

### Milestone 3: "Textured World" (Semana 3)
![Textures](docs/images/milestone3_textures.png)
- Objetos texturizados
- Câmera FPS funcional
- Múltiplos materiais

### Milestone 4: "3D Models" (Semana 4)
![Models](docs/images/milestone4_models.png)
- Modelos complexos carregados
- Depth buffer correto
- Performance otimizada

### Milestone 5: "Lit Scene" (Semana 5-6)
![Lighting](docs/images/milestone5_lighting.png)
- Iluminação realista
- Múltiplas luzes
- Normal mapping

### Milestone 6: "Interactive Game" (Semana 7-8)
![Game](docs/images/milestone6_game.png)
- Player controller
- ECS system
- Game loop completo

### Milestone 7: "Production Ready" (Semana 9-12)
![Advanced](docs/images/milestone7_advanced.png)
- Shadows
- Post-processing
- PBR materials
- 10k+ objects

---

## 📋 DAILY CHECKLIST TEMPLATE

```markdown
## [DATE] - Dia X da Semana Y

### 🎯 Objetivo do Dia
[Descrição do que será implementado]

### ✅ Tarefas
- [ ] Tarefa 1
- [ ] Tarefa 2
- [ ] Tarefa 3

### 📊 Progresso
- Linhas adicionadas: XXX
- Linhas removidas: XXX
- Arquivos modificados: X

### 🐛 Bugs Encontrados
1. [Descrição do bug]
   - Status: [Resolvido/Pendente]
   - Solução: [Como foi resolvido]

### 🧪 Testes
- [ ] Compila sem warnings
- [ ] Valgrind clean
- [ ] Validation layers OK
- [ ] Feature funciona como esperado

### 📝 Notas
[Observações, decisões de design, etc.]

### 📸 Screenshots/Videos
[Links para capturas do progresso]

### ⏰ Tempo Gasto
- Planejado: Xh
- Real: Xh
- Diferença: +/-Xh

### 🔜 Próximo Passo
[O que fazer amanhã]
```

---

## 🔄 PROCESSO DE REFATORAÇÃO CONTÍNUA

### Quando Refatorar?
1. **Code Smell Detection:**
   - Funções > 50 linhas
   - Classes > 500 linhas
   - Duplicação de código (DRY)
   - Acoplamento alto

2. **Performance Issues:**
   - Frame drops
   - Memory leaks
   - GPU stalls

3. **Architecture Debt:**
   - Singletons proliferando
   - God objects crescendo
   - Responsabilidades misturadas

### Como Refatorar?
1. **Escrever testes primeiro** (se possível)
2. **Pequenas mudanças incrementais**
3. **Commit após cada mudança funcional**
4. **Validar com Valgrind + Validation Layers**
5. **Benchmark antes/depois**

### Refactoring Patterns
```cpp
// Pattern 1: Extract Class
// ANTES: VulkanManager com 3000 linhas
// DEPOIS: VulkanManager + ResourceManager + BufferManager + ...

// Pattern 2: Dependency Injection
// ANTES: Singleton getInstance()
// DEPOIS: Construtor recebe referências

// Pattern 3: Strategy Pattern
// ANTES: if/else para diferentes materiais
// DEPOIS: Interface Material com implementações

// Pattern 4: Handle/ID Pattern
// ANTES: Passar VkBuffer por todo lado
// DEPOIS: BufferHandle (ID opaco)
```

---

## 🧪 TESTING STRATEGY

### Unit Tests
```cpp
// Exemplo: Testar BufferManager
TEST(BufferManager, CreateVertexBuffer) {
    BufferManager bufferMgr(device, allocator, resources, commands);
    
    std::vector<float> vertices = {0,0,0, 1,0,0, 0,1,0};
    BufferHandle handle = bufferMgr.createVertexBuffer(
        vertices.data(), 
        vertices.size() * sizeof(float)
    );
    
    ASSERT_NE(handle, INVALID_HANDLE);
    ASSERT_EQ(resources.getBufferSize(handle), vertices.size() * sizeof(float));
}
```

### Integration Tests
```cpp
// Exemplo: Testar pipeline completo
TEST(Integration, RenderTriangle) {
    // Setup
    VulkanManager vulkan(800, 600, "Test");
    vulkan.initVulkan();
    
    // Render um frame
    vulkan.drawFrame();
    
    // Capturar framebuffer
    std::vector<uint8_t> pixels = vulkan.captureFramebuffer();
    
    // Validar que não está preto (algo foi desenhado)
    bool hasColor = std::any_of(pixels.begin(), pixels.end(), 
                                 [](uint8_t p) { return p > 10; });
    ASSERT_TRUE(hasColor);
}
```

### Performance Tests
```cpp
// Exemplo: Benchmark de draw calls
BENCHMARK(DrawCalls, 1000Objects) {
    for (int i = 0; i < 1000; ++i) {
        scene.addObject(createCube());
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    renderer.render(scene);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    ASSERT_LT(duration.count(), 16);  // < 16ms (60 FPS)
}
```

### Smoke Tests (CI)
```bash
#!/bin/bash
# Smoke test para CI

# Build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build

# Run headless (sem janela)
./build/Speed_Racer --test --headless

# Verificar exit code
if [ $? -ne 0 ]; then
    echo "Smoke test failed!"
    exit 1
fi

echo "Smoke test passed!"
```

---

## 📈 MÉTRICAS DE SUCESSO (KPIs)

### Performance
- **Target FPS:** 60+ @ 1080p, 30+ @ 4K
- **Frame Time:** < 16.67ms (60 FPS)
- **Memory Usage:** < 2GB para cena típica
- **Load Time:** < 5s para cena complexa

### Quality
- **Code Coverage:** > 70%
- **Warnings:** 0
- **Memory Leaks:** 0
- **Validation Errors:** 0

### Architecture
- **Average Function Size:** < 30 linhas
- **Average Class Size:** < 300 linhas
- **Cyclomatic Complexity:** < 10 por função
- **Coupling:** Low (< 5 dependencies per class)

### Developer Experience
- **Build Time:** < 30s (incremental)
- **Hot Reload Time:** < 1s (shaders/assets)
- **Documentation Coverage:** > 80%
- **Onboarding Time:** < 1 dia (novo dev pode contribuir)

---

## 🎓 LIÇÕES APRENDIDAS (TEMPLATE)

```markdown
## Lição: [Título]

### Contexto
[O que você estava tentando fazer]

### Problema
[O que deu errado]

### Solução
[Como você resolveu]

### Code Example
```cpp
// ANTES (problemático)
[código antigo]

// DEPOIS (correto)
[código novo]
```

### Takeaway
[Lição principal para o futuro]

### Referências
- [Link 1]
- [Link 2]
```

---

## 🚀 QUICK START GUIDE

### Setup Inicial (5 minutos)
```bash
# Clone
git clone https://github.com/seu-usuario/speed-racer
cd speed-racer

# Instalar dependências (Ubuntu/Debian)
sudo apt install vulkan-sdk cmake ninja-build

# Submodules
git submodule update --init --recursive

# Build
cmake -B build -G Ninja
cmake --build build

# Run
./build/Speed_Racer
```

### Desenvolvimento Diário
```bash
# 1. Pull latest
git pull origin main

# 2. Criar branch para feature
git checkout -b feature/vertex-buffers

# 3. Desenvolver...
# (edit código)

# 4. Build incremental
cmake --build build

# 5. Test
./build/Speed_Racer

# 6. Commit
git add .
git commit -m "feat: add vertex buffer support"

# 7. Push
git push origin feature/vertex-buffers

# 8. Open PR no GitHub
```

---

## 🎯 CONCLUSÃO E PRÓXIMOS PASSOS

### ✅ O Que Mudou na Revisão?

1. **❌ Removido:** Singletons → **✅ Adicionado:** Dependency Injection
2. **❌ Removido:** Gerenciamento manual de memória → **✅ Adicionado:** VMA obrigatório
3. **🆕 Adicionado:** Shader toolchain no CMake
4. **🆕 Adicionado:** Frames-in-flight validação formal
5. **🆕 Adicionado:** Scene Graph antecipado (Semana 2)
6. **🆕 Adicionado:** Mini FrameGraph (Semana 2)
7. **🆕 Adicionado:** CI/CD básico
8. **🆕 Adicionado:** Definições de Done mensuráveis
9. **🆕 Adicionado:** Debug utils (labels, timestamps)
10. **🆕 Adicionado:** Ferramentas de profiling

### 🎯 Seu Próximo Passo IMEDIATO

**COMECE POR AQUI:**
```bash
# 1. Integrar VMA (Dia 1, Semana 1)
cd libs
git clone https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
cd ..

# 2. Atualizar CMakeLists.txt
# (adicionar VMA como dependência)

# 3. Criar VmaWrapper.hpp
# (começar abstração de memória)
```

### 📊 Roadmap Visual

```
MÊS 1                    MÊS 2                    MÊS 3
│                        │                        │
├─ Semana 1: Fundação    ├─ Semana 5-6: Lighting  ├─ Semana 9-10: Shadows/PP
├─ Semana 2: Geometria   ├─ Semana 7-8: ECS/Input ├─ Semana 11-12: PBR/Inst
├─ Semana 3: Textures    │                        │
├─ Semana 4: Models      │                        │
│                        │                        │
▼                        ▼                        ▼
25% Complete          → 50% Complete          → 75% Complete
```

### 🏆 Meta Final

**Ao final das 12 semanas, você terá:**
- ✅ Engine Vulkan moderna e profissional
- ✅ Arquitetura limpa (DI, RAII, handles)
- ✅ Performance otimizada (60+ FPS)
- ✅ Features avançadas (PBR, shadows, instancing)
- ✅ Tooling completo (CI, profiling, hot-reload)
- ✅ Documentação robusta
- ✅ Base sólida para construir jogos AAA

---

## 📞 SUPORTE E COMUNIDADE

### Onde Buscar Ajuda
- **Discord:** [Vulkan Community](https://discord.gg/vulkan)
- **Reddit:** r/vulkan, r/gamedev
- **Stack Overflow:** Tag `vulkan`
- **GitHub Issues:** Para bugs específicos do projeto

### Como Contribuir
1. Fork o projeto
2. Crie uma branch (`git checkout -b feature/amazing-feature`)
3. Commit suas mudanças (`git commit -m 'feat: add amazing feature'`)
4. Push para a branch (`git push origin feature/amazing-feature`)
5. Abra um Pull Request

---

## 📄 LICENSE

Este projeto está sob a licença MIT. Veja o arquivo `LICENSE` para mais detalhes.

---

## 🙏 AGRADECIMENTOS

- Vulkan Tutorial (Alexander Overvoorde)
- Sascha Willems (Vulkan Examples)
- Khronos Group (Vulkan Specification)
- Comunidade Vulkan no Discord
- Todos os contributors

---

## 📝 CHANGELOG

### v2.0.0 (Revisado) - 2025-01-XX
- **BREAKING:** Removido Singletons, adicionado DI
- **NEW:** VMA obrigatório
- **NEW:** Shader toolchain no CMake
- **NEW:** Scene Graph antecipado
- **NEW:** Mini FrameGraph
- **NEW:** CI/CD básico
- **IMPROVED:** Definições de Done mensuráveis
- **IMPROVED:** Debug utils e profiling

### v1.0.0 (Original) - 2025-01-XX
- Roadmap inicial
- 4 semanas de features core
- Estrutura básica definida

---

**🏎️ Agora é hora de acelerar! Boa sorte no desenvolvimento! 🏁**

```
     ___
    |  _|___  ____  ____  ____
    | |_|___||  _ \|  __||  _ \
    |___|    |____/|____||____/
    
    Speed Racer Engine v2.0
    "Built for Speed, Designed for Performance"
```---

## 🔥 APÊNDICE A: PADRÕES DE CÓDIGO E CONVENÇÕES

### Naming Conventions

```cpp
// Classes: PascalCase
class BufferManager { };
class VulkanRenderer { };

// Functions/Methods: camelCase
void createBuffer();
void updateDescriptorSets();

// Variables: camelCase
int frameCount;
VkDevice logicalDevice;

// Constants: SCREAMING_SNAKE_CASE
const int MAX_FRAMES_IN_FLIGHT = 2;
const float PI = 3.14159f;

// Private Members: camelCase com prefixo 'm_' (opcional)
class Example {
private:
    int m_count;  // ou apenas 'count'
    VkDevice m_device;
};

// Handles: sufixo 'Handle'
using BufferHandle = uint32_t;
using TextureHandle = uint64_t;

// Managers/Systems: sufixo 'Manager' ou 'System'
class ResourceManager { };
class RenderSystem { };

// Enums: PascalCase com valores SCREAMING_SNAKE_CASE
enum class ShaderStage {
    VERTEX,
    FRAGMENT,
    COMPUTE
};
```

### File Organization

```
Speed_Racer/
├── assets/                      # Assets (não compilados)
│   ├── shaders/
│   │   ├── core/
│   │   │   ├── basic.vert       # Shader source
│   │   │   └── basic.frag
│   │   └── lighting/
│   ├── textures/
│   ├── models/
│   └── config/
│       ├── engine.json
│       └── input.json
├── build/                       # Build artifacts (gitignore)
│   ├── shaders/                 # Compiled shaders (.spv)
│   └── Speed_Racer              # Executable
├── docs/                        # Documentação
│   ├── architecture.md
│   ├── api/
│   └── images/
├── include/                     # Headers públicos
│   ├── core/
│   │   ├── VulkanManager.hpp
│   │   ├── BufferManager.hpp
│   │   └── ResourceManager.hpp
│   ├── rendering/
│   │   ├── Renderer.hpp
│   │   ├── Material.hpp
│   │   └── Mesh.hpp
│   ├── scene/
│   │   ├── Scene.hpp
│   │   ├── SceneNode.hpp
│   │   └── Transform.hpp
│   └── ecs/
│       ├── Component.hpp
│       └── System.hpp
├── src/                         # Implementações
│   ├── core/
│   ├── rendering/
│   ├── scene/
│   └── app/
│       └── main.cpp
├── libs/                        # Dependências externas
│   ├── glfw/
│   ├── VulkanMemoryAllocator/
│   ├── glm/
│   ├── stb/
│   ├── assimp/
│   └── imgui/
├── tests/                       # Testes unitários/integração
│   ├── unit/
│   └── integration/
├── tools/                       # Scripts utilitários
│   ├── build_and_run.sh
│   ├── compile_shaders.sh
│   └── valgrind_check.sh
├── .github/
│   └── workflows/
│       └── ci.yml
├── CMakeLists.txt
├── README.md
├── LICENSE
└── .gitignore
```

### Header Guards

```cpp
// OPÇÃO 1: Include Guards (tradicional)
#ifndef SPEED_RACER_BUFFER_MANAGER_HPP
#define SPEED_RACER_BUFFER_MANAGER_HPP

// ... conteúdo ...

#endif // SPEED_RACER_BUFFER_MANAGER_HPP

// OPÇÃO 2: #pragma once (moderno, mais simples)
#pragma once

// ... conteúdo ...
```

### Include Order

```cpp
// 1. Header correspondente (se .cpp)
#include "BufferManager.hpp"

// 2. C system headers
#include <cstdint>
#include <cstring>

// 3. C++ standard library
#include <vector>
#include <memory>
#include <algorithm>

// 4. Bibliotecas externas
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

// 5. Headers do projeto (outras partes)
#include <core/ResourceManager.hpp>
#include <core/VulkanTools.hpp>
```

### Code Formatting

```cpp
// Indentação: 4 espaços (não tabs)
// Chaves: K&R style (ou Allman, mas seja consistente)

// K&R Style (RECOMENDADO):
class Example {
public:
    void function() {
        if (condition) {
            // código
        } else {
            // código
        }
    }
};

// Allman Style (alternativa):
class Example
{
public:
    void function()
    {
        if (condition)
        {
            // código
        }
        else
        {
            // código
        }
    }
};

// Line Length: máximo 120 caracteres
// Se necessário quebrar, indentar continuação com +4 espaços

void longFunctionName(
    VkDevice device,
    const VkBufferCreateInfo& bufferInfo,
    const VmaAllocationCreateInfo& allocInfo) 
{
    // ...
}
```

### Comments

```cpp
// Comentários de linha única: explicar "por quê", não "o quê"

// ❌ MAU:
int count = 0;  // Inicializa count com 0

// ✅ BOM:
int count = 0;  // Start from zero to include all items in first batch

// Comentários de bloco para documentação de funções/classes:

/**
 * @brief Cria um buffer Vulkan com alocação via VMA
 * 
 * @param size Tamanho do buffer em bytes
 * @param usage Flags de uso (VK_BUFFER_USAGE_*)
 * @param memoryUsage Tipo de memória VMA (GPU/CPU/Staging)
 * @return BufferHandle Handle para o buffer criado
 * 
 * @throws std::runtime_error Se a criação falhar
 * 
 * @note Este método automaticamente registra o buffer no ResourceManager
 * @see ResourceManager::registerBuffer()
 */
BufferHandle createBuffer(
    size_t size,
    VkBufferUsageFlags usage,
    VmaMemoryUsage memoryUsage
);

// TODO comments para trabalho futuro:
// TODO(seu-nome): Implementar cache de pipelines em disco
// FIXME(seu-nome): Memory leak ao destruir swapchain
// HACK(seu-nome): Workaround temporário para bug do driver AMD
// NOTE(seu-nome): Este código depende do comportamento não documentado X
```

### Error Handling

```cpp
// OPÇÃO 1: Exceptions (recomendado para erros irrecuperáveis)
void createBuffer() {
    VkResult result = vkCreateBuffer(device, &info, nullptr, &buffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error(
            "[BufferManager] Failed to create buffer: " + 
            vkResultToString(result)
        );
    }
}

// OPÇÃO 2: Result/Optional para erros recuperáveis
std::optional<BufferHandle> tryCreateBuffer() {
    VkResult result = vkCreateBuffer(device, &info, nullptr, &buffer);
    if (result != VK_SUCCESS) {
        logError("[BufferManager] Failed to create buffer");
        return std::nullopt;
    }
    return registerBuffer(buffer);
}

// OPÇÃO 3: Expected/Result type (C++23 ou biblioteca externa)
Expected<BufferHandle, VkResult> createBuffer() {
    VkResult result = vkCreateBuffer(device, &info, nullptr, &buffer);
    if (result != VK_SUCCESS) {
        return Unexpected(result);
    }
    return registerBuffer(buffer);
}

// Uso:
auto result = createBuffer();
if (result.has_value()) {
    BufferHandle handle = result.value();
} else {
    handleError(result.error());
}
```

### RAII Wrappers

```cpp
// Sempre usar RAII para recursos Vulkan

// ❌ MAU:
VkBuffer buffer;
vkCreateBuffer(device, &info, nullptr, &buffer);
// ... usar buffer ...
vkDestroyBuffer(device, buffer, nullptr);  // Pode ser esquecido!

// ✅ BOM:
class ScopedBuffer {
public:
    ScopedBuffer(VkDevice device, const VkBufferCreateInfo& info)
        : device(device) {
        if (vkCreateBuffer(device, &info, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create buffer");
        }
    }
    
    ~ScopedBuffer() {
        if (buffer != VK_NULL_HANDLE) {
            vkDestroyBuffer(device, buffer, nullptr);
        }
    }
    
    // Delete copy, allow move
    ScopedBuffer(const ScopedBuffer&) = delete;
    ScopedBuffer& operator=(const ScopedBuffer&) = delete;
    
    ScopedBuffer(ScopedBuffer&& other) noexcept
        : device(other.device), buffer(other.buffer) {
        other.buffer = VK_NULL_HANDLE;
    }
    
    VkBuffer get() const { return buffer; }
    
private:
    VkDevice device;
    VkBuffer buffer = VK_NULL_HANDLE;
};

// Uso:
{
    ScopedBuffer buffer(device, info);
    // ... usar buffer ...
}  // Automaticamente destruído
```

---

## 🔥 APÊNDICE B: VULKAN BEST PRACTICES

### Memory Management

```cpp
// 1. Use VMA para TUDO relacionado a memória
VmaAllocator allocator;
VmaAllocatorCreateInfo allocatorInfo = {};
allocatorInfo.device = device;
allocatorInfo.physicalDevice = physicalDevice;
allocatorInfo.instance = instance;
vmaCreateAllocator(&allocatorInfo, &allocator);

// 2. Escolha o tipo de memória correto
VmaAllocationCreateInfo allocInfo = {};

// Para buffers que CPU escreve e GPU lê (Staging, Uniform)
allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

// Para buffers que só GPU usa (Vertex, Index)
allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

// Para buffers que GPU escreve e CPU lê (Readback)
allocInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;

// 3. Prefira buffers grandes com offsets ao invés de muitos pequenos
// ❌ MAU: 1000 buffers pequenos
for (int i = 0; i < 1000; ++i) {
    VkBuffer buffer;
    vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, ...);
}

// ✅ BOM: 1 buffer grande com offsets
VkDeviceSize totalSize = 1000 * objectSize;
VkBuffer bigBuffer;
vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &bigBuffer, ...);
// Usar offsets: (i * objectSize)
```

### Pipeline State

```cpp
// 1. Minimize pipeline switches
// Agrupe objetos por pipeline antes de desenhar

struct DrawBatch {
    VkPipeline pipeline;
    std::vector<DrawCommand> commands;
};

std::unordered_map<VkPipeline, DrawBatch> batches;

// Collect
for (auto& obj : objects) {
    batches[obj.pipeline].commands.push_back(obj.drawCmd);
}

// Draw
for (auto& [pipeline, batch] : batches) {
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    for (auto& cmd : batch.commands) {
        vkCmdDraw(...);
    }
}

// 2. Use pipeline derivatives para variações
VkGraphicsPipelineCreateInfo baseInfo = { /* ... */ };
baseInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;

VkPipeline basePipeline;
vkCreateGraphicsPipelines(device, cache, 1, &baseInfo, nullptr, &basePipeline);

// Criar variação (ex: wireframe)
VkGraphicsPipelineCreateInfo derivedInfo = baseInfo;
derivedInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
derivedInfo.basePipelineHandle = basePipeline;
derivedInfo.basePipelineIndex = -1;
derivedInfo.pRasterizationState->polygonMode = VK_POLYGON_MODE_LINE;

VkPipeline derivedPipeline;
vkCreateGraphicsPipelines(device, cache, 1, &derivedInfo, nullptr, &derivedPipeline);
```

### Descriptor Sets

```cpp
// 1. Use descriptor indexing (Vulkan 1.2+) para flexibilidade
VkDescriptorSetLayoutBinding binding = {};
binding.binding = 0;
binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
binding.descriptorCount = 1000;  // Array de texturas!
binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

// Shader:
// layout(set = 0, binding = 0) uniform sampler2D textures[1000];
// 
// void main() {
//     int texIndex = materialData.textureIndex;
//     vec4 color = texture(textures[texIndex], uv);
// }

// 2. Organize descriptor sets por frequência de update
// Set 0: Por frame (camera, lights) - update todo frame
// Set 1: Por material (textures) - update ao trocar material
// Set 2: Por objeto (transforms) - update por objeto

layout(set = 0, binding = 0) uniform FrameUBO { ... };
layout(set = 1, binding = 0) uniform sampler2D albedoMap;
layout(set = 2, binding = 0) uniform ObjectUBO { ... };

// Bind apenas os sets que mudaram:
vkCmdBindDescriptorSets(cmd, ..., 0, 1, &frameSet, ...);  // Bind set 0
// ... desenhar vários objetos com mesmo material ...
vkCmdBindDescriptorSets(cmd, ..., 1, 1, &materialSet, ...);  // Bind set 1
// ... desenhar vários objetos ...
vkCmdBindDescriptorSets(cmd, ..., 2, 1, &objectSet, ...);  // Bind set 2 por objeto
```

### Synchronization

```cpp
// 1. Use barriers corretamente para transições de layout
void transitionImageLayout(
    VkCommandBuffer cmd,
    VkImage image,
    VkImageLayout oldLayout,
    VkImageLayout newLayout)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && 
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } 
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && 
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    // ... outros casos ...

    vkCmdPipelineBarrier(
        cmd,
        srcStage, dstStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
}

// 2. Timeline semaphores para sincronização mais granular (Vulkan 1.2+)
VkSemaphoreTypeCreateInfo timelineInfo = {};
timelineInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
timelineInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
timelineInfo.initialValue = 0;

VkSemaphoreCreateInfo semaphoreInfo = {};
semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
semaphoreInfo.pNext = &timelineInfo;

VkSemaphore timelineSemaphore;
vkCreateSemaphore(device, &semaphoreInfo, nullptr, &timelineSemaphore);

// Signal com valor
VkTimelineSemaphoreSubmitInfo timelineSubmitInfo = {};
timelineSubmitInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
timelineSubmitInfo.signalSemaphoreValueCount = 1;
uint64_t signalValue = 1;
timelineSubmitInfo.pSignalSemaphoreValues = &signalValue;

VkSubmitInfo submitInfo = {};
submitInfo.pNext = &timelineSubmitInfo;
// ... resto do submit ...

// Wait no host
VkSemaphoreWaitInfo waitInfo = {};
waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
waitInfo.semaphoreCount = 1;
waitInfo.pSemaphores = &timelineSemaphore;
uint64_t waitValue = 1;
waitInfo.pValues = &waitValue;

vkWaitSemaphores(device, &waitInfo, UINT64_MAX);
```

### Command Buffers

```cpp
// 1. Use command buffer secundários para multi-threading
// Thread principal
VkCommandBuffer primaryCmd;
vkBeginCommandBuffer(primaryCmd, ...);

vkCmdBeginRenderPass(primaryCmd, ...);

// Threads secundários (podem rodar em paralelo)
std::vector<VkCommandBuffer> secondaryCmds(numThreads);
std::vector<std::thread> threads;

for (int i = 0; i < numThreads; ++i) {
    threads.emplace_back([&, i]() {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
        beginInfo.pInheritanceInfo = &inheritanceInfo;
        
        vkBeginCommandBuffer(secondaryCmds[i], &beginInfo);
        // ... record draw commands ...
        vkEndCommandBuffer(secondaryCmds[i]);
    });
}

for (auto& t : threads) t.join();

// Execute secondary command buffers
vkCmdExecuteCommands(primaryCmd, numThreads, secondaryCmds.data());

vkCmdEndRenderPass(primaryCmd);
vkEndCommandBuffer(primaryCmd);

// 2. Reset command pool ao invés de command buffers individuais
// ❌ MAU:
for (auto& cmd : commandBuffers) {
    vkResetCommandBuffer(cmd, 0);
}

// ✅ BOM:
vkResetCommandPool(device, commandPool, 0);  // Reseta todos de uma vez
```

### Performance Tips

```cpp
// 1. Batch uploads para evitar múltiplos staging buffers
struct UploadBatch {
    std::vector<std::pair<void*, size_t>> data;  // data pointer, size
    std::vector<VkBuffer> dstBuffers;
    std::vector<VkDeviceSize> dstOffsets;
};

void flushUploadBatch(const UploadBatch& batch) {
    // 1. Calcular tamanho total
    VkDeviceSize totalSize = 0;
    for (auto& [data, size] : batch.data) {
        totalSize += size;
    }
    
    // 2. Criar staging buffer único
    VkBuffer stagingBuffer = createStagingBuffer(totalSize);
    
    // 3. Map e copiar todos os dados
    void* mapped;
    vmaMapMemory(allocator, stagingAlloc, &mapped);
    
    VkDeviceSize offset = 0;
    for (auto& [data, size] : batch.data) {
        memcpy((char*)mapped + offset, data, size);
        offset += size;
    }
    
    vmaUnmapMemory(allocator, stagingAlloc);
    
    // 4. Um único command buffer para todas as cópias
    VkCommandBuffer cmd = beginSingleTimeCommands();
    
    offset = 0;
    for (size_t i = 0; i < batch.data.size(); ++i) {
        VkBufferCopy region = {};
        region.srcOffset = offset;
        region.dstOffset = batch.dstOffsets[i];
        region.size = batch.data[i].second;
        
        vkCmdCopyBuffer(cmd, stagingBuffer, batch.dstBuffers[i], 1, &region);
        
        offset += batch.data[i].second;
    }
    
    endSingleTimeCommands(cmd);
    
    // 5. Limpar staging buffer
    vmaDestroyBuffer(allocator, stagingBuffer, stagingAlloc);
}

// 2. Use push constants para dados pequenos e frequentes
struct PushConstants {
    glm::mat4 model;
    uint32_t textureIndex;
};

VkPushConstantRange pushRange = {};
pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
pushRange.offset = 0;
pushRange.size = sizeof(PushConstants);

// No render loop:
PushConstants pc;
pc.model = object.transform;
pc.textureIndex = object.materialIndex;

vkCmdPushConstants(cmd, pipelineLayout, pushRange.stageFlags, 0, sizeof(pc), &pc);
vkCmdDrawIndexed(...);

// 3. Use indirect drawing para GPU-driven rendering
struct VkDrawIndexedIndirectCommand {
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  vertexOffset;
    uint32_t firstInstance;
};

// Preencher buffer de commands (pode ser feito pela GPU!)
std::vector<VkDrawIndexedIndirectCommand> commands;
for (auto& obj : visibleObjects) {
    commands.push_back({
        obj.indexCount,
        1,  // instanceCount
        obj.firstIndex,
        obj.vertexOffset,
        0
    });
}

BufferHandle indirectBuffer = bufferMgr.createBuffer(
    commands.data(),
    commands.size() * sizeof(VkDrawIndexedIndirectCommand),
    VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT
);

// Draw com um único comando!
vkCmdDrawIndexedIndirect(
    cmd,
    indirectBuffer,
    0,  // offset
    commands.size(),  // drawCount
    sizeof(VkDrawIndexedIndirectCommand)  // stride
);
```

---

## 🔥 APÊNDICE C: DEBUGGING E PROFILING

### RenderDoc Integration

```cpp
// 1. Trigger RenderDoc capture programaticamente
#ifdef USE_RENDERDOC
#include <renderdoc_app.h>

RENDERDOC_API_1_1_2 *rdoc_api = nullptr;

void initRenderDoc() {
    #ifdef _WIN32
    HMODULE mod = GetModuleHandleA("renderdoc.dll");
    #else
    void* mod = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
    #endif
    
    if (mod) {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI =
            (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
        RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api);
    }
}

void captureFrame() {
    if (rdoc_api) {
        rdoc_api->TriggerCapture();
    }
}
#endif

// Uso:
if (input.isKeyPressed(Key::F11)) {
    captureFrame();  // Captura o próximo frame
}
```

### Validation Layers Customizadas

```cpp
// Callback customizado para validation layers
VkBool32 debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    // Filtrar mensagens específicas
    if (strstr(pCallbackData->pMessage, "UNASSIGNED-CoreValidation-DrawState-InvalidImageLayout")) {
        // Ignorar este warning específico (se for conhecido e inofensivo)
        return VK_FALSE;
    }
    
    // Colorir output baseado em severidade
    const char* color = "";
    const char* reset = "\033[0m";
    
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        color = "\033[31m";  // Vermelho
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        color = "\033[33m";  // Amarelo
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        color = "\033[36m";  // Ciano
    }
    
    std::cerr << color << "[VULKAN] " << pCallbackData->pMessage << reset << std::endl;
    
    // Break no debugger se for erro crítico
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        #ifdef _DEBUG
        __debugbreak();  // MSVC
        // __builtin_trap();  // GCC/Clang
        #endif
    }
    
    return VK_FALSE;
}
```

### GPU Timestamps

```cpp
class GPUProfiler {
public:
    void initialize(VkDevice device, VkPhysicalDevice physDevice, uint32_t queueFamily) {
        // Criar query pool
        VkQueryPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        poolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
        poolInfo.queryCount = MAX_QUERIES;
        
        vkCreateQueryPool(device, &poolInfo, nullptr, &queryPool);
        
        // Get timestamp period
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physDevice, &props);
        timestampPeriod = props.limits.timestampPeriod;
    }
    
    void beginRegion(VkCommandBuffer cmd, const std::string& name) {
        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
                           queryPool, queryIndex++);
        regionNames.push_back(name);
    }
    
    void endRegion(VkCommandBuffer cmd) {
        vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
                           queryPool, queryIndex++);
    }
    
    void printResults() {
        std::vector<uint64_t> timestamps(queryIndex);
        vkGetQueryPoolResults(device, queryPool, 0, queryIndex,
                             timestamps.size() * sizeof(uint64_t),
                             timestamps.data(), sizeof(uint64_t),
                             VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
        
        std::cout << "=== GPU Timings ===" << std::endl;
        for (size_t i = 0; i < regionNames.size(); ++i) {
            uint64_t start = timestamps[i * 2];
            uint64_t end = timestamps[i * 2 + 1];
            float ms = (end - start) * timestampPeriod / 1000000.0f;
            std::cout << regionNames[i] << ": " << ms << " ms" << std::endl;
        }
        
        // Reset para próximo frame
        vkCmdResetQueryPool(cmd, queryPool, 0, queryIndex);
        queryIndex = 0;
        regionNames.clear();
    }
    
private:
    VkQueryPool queryPool;
    uint32_t queryIndex = 0;
    float timestampPeriod;
    std::vector<std::string> regionNames;
    static const uint32_t MAX_QUERIES = 256;
};

// Uso:
gpuProfiler.beginRegion(cmd, "Shadow Pass");
renderShadowMap(cmd);
gpuProfiler.endRegion(cmd);

gpuProfiler.beginRegion(cmd, "Main Pass");
renderMainPass(cmd);
gpuProfiler.endRegion(cmd);

// No final do frame:
gpuProfiler.printResults();
```

### Tracy Profiler Integration

```cpp
// 1. Adicionar TracyVulkan.hpp ao projeto
#include <tracy/TracyVulkan.hpp>

// 2. Criar contexto Tracy
TracyVkCtx tracyCtx = TracyVkContext(
    physicalDevice,
    device,
    queue,
    commandBuffer
);

// 3. Instrumentar código
void renderFrame() {
    ZoneScoped;  // CPU profiling
    
    VkCommandBuffer cmd = beginFrame();
    
    {
        TracyVkZone(tracyCtx, cmd, "Shadow Pass");
        renderShadowMap(cmd);
    }
    
    {
        TracyVkZone(tracyCtx, cmd, "Main Pass");
        renderMainPass(cmd);
    }
    
    {
        TracyVkZone(tracyCtx, cmd, "UI Pass");
        renderUI(cmd);
    }
    
    endFrame(cmd);
    
    // Collect Tracy data
    TracyVkCollect(tracyCtx, cmd);
}

// 4. Frame marking
void mainLoop() {
    while (!shouldClose) {
        FrameMark;  // Tracy frame marker
        
        update(deltaTime);
        render();
    }
}

// 5. Custom plots
TracyPlot("FPS", fps);
TracyPlot("Frame Time (ms)", frameTimeMs);
TracyPlot("Draw Calls", drawCallCount);
TracyPlot("Triangles", triangleCount);
```

### Memory Leak Detection

```cpp
// 1. VMA Statistics
void printMemoryStats() {
    VmaBudget budgets[VK_MAX_MEMORY_HEAPS];
    vmaGetHeapBudgets(allocator, budgets);
    
    std::cout << "=== Memory Usage ===" << std::endl;
    for (uint32_t i = 0; i < VK_MAX_MEMORY_HEAPS; ++i) {
        if (budgets[i].usage > 0) {
            float usageMB = budgets[i].usage / (1024.0f * 1024.0f);
            float budgetMB = budgets[i].budget / (1024.0f * 1024.0f);
            std::cout << "Heap " << i << ": " 
                      << usageMB << " MB / " << budgetMB << " MB"
                      << " (" << (usageMB / budgetMB * 100.0f) << "%)" 
                      << std::endl;
        }
    }
    
    // Detailed stats
    VmaDetailedStatistics stats;
    vmaCalculateStatistics(allocator, &stats);
    
    std::cout << "Total Allocations: " << stats.statistics.allocationCount << std::endl;
    std::cout << "Total Blocks: " << stats.statistics.blockCount << std::endl;
}

// 2. Resource Tracking Customizado
class ResourceTracker {
public:
    void trackAllocation(const std::string& name, size_t size, const char* file, int line) {
        std::lock_guard<std::mutex> lock(mutex);
        allocations[name] = {size, file, line, std::chrono::system_clock::now()};
        totalAllocated += size;
    }
    
    void trackDeallocation(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = allocations.find(name);
        if (it != allocations.end()) {
            totalAllocated -= it->second.size;
            allocations.erase(it);
        }
    }
    
    void printLeaks() {
        std::lock_guard<std::mutex> lock(mutex);
        if (allocations.empty()) {
            std::cout << "No memory leaks detected!" << std::endl;
            return;
        }
        
        std::cout << "=== MEMORY LEAKS DETECTED ===" << std::endl;
        for (auto& [name, info] : allocations) {
            std::cout << name << ": " << info.size << " bytes" << std::endl;
            std::cout << "  Allocated at: " << info.file << ":" << info.line << std::endl;
            
            auto now = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - info.timestamp);
            std::cout << "  Age: " << duration.count() << " seconds" << std::endl;
        }
        std::cout << "Total leaked: " << totalAllocated << " bytes" << std::endl;
    }
    
private:
    struct AllocationInfo {
        size_t size;
        const char* file;
        int line;
        std::chrono::time_point<std::chrono::system_clock> timestamp;
    };
    
    std::unordered_map<std::string, AllocationInfo> allocations;
    size_t totalAllocated = 0;
    std::mutex mutex;
};

// Uso:
#define TRACK_ALLOC(name, size) resourceTracker.trackAllocation(name, size, __FILE__, __LINE__)
#define TRACK_DEALLOC(name) resourceTracker.trackDeallocation(name)

BufferHandle createBuffer(...) {
    // ... criar buffer ...
    TRACK_ALLOC("Buffer_" + std::to_string(handle), size);
    return handle;
}

void destroyBuffer(BufferHandle handle) {
    TRACK_DEALLOC("Buffer_" + std::to_string(handle));
    // ... destruir buffer ...
}

// No final da aplicação:
resourceTracker.printLeaks();
```

---

## 🔥 APÊNDICE D: OTIMIZAÇÕES AVANÇADAS

### Frustum Culling Implementation

```cpp
class Frustum {
public:
    // Planos: Left, Right, Top, Bottom, Near, Far
    std::array<glm::vec4, 6> planes;
    
    void update(const glm::mat4& viewProj) {
        // Extract frustum planes from view-projection matrix
        // Left
        planes[0] = glm::vec4(
            viewProj[0][3] + viewProj[0][0],
            viewProj[1][3] + viewProj[1][0],
            viewProj[2][3] + viewProj[2][0],
            viewProj[3][3] + viewProj[3][0]
        );
        
        // Right
        planes[1] = glm::vec4(
            viewProj[0][3] - viewProj[0][0],
            viewProj[1][3] - viewProj[1][0],
            viewProj[2][3] - viewProj[2][0],
            viewProj[3][3] - viewProj[3][0]
        );
        
        // Bottom
        planes[2] = glm::vec4(
            viewProj[0][3] + viewProj[0][1],
            viewProj[1][3] + viewProj[1][1],
            viewProj[2][3] + viewProj[2][1],
            viewProj[3][3] + viewProj[3][1]
        );
        
        // Top
        planes[3] = glm::vec4(
            viewProj[0][3] - viewProj[0][1],
            viewProj[1][3] - viewProj[1][1],
            viewProj[2][3] - viewProj[2][1],
            viewProj[3][3] - viewProj[3][1]
        );
        
        // Near
        planes[4] = glm::vec4(
            viewProj[0][3] + viewProj[0][2],
            viewProj[1][3] + viewProj[1][2],
            viewProj[2][3] + viewProj[2][2],
            viewProj[3][3] + viewProj[3][2]
        );
        
        // Far
        planes[5] = glm::vec4(
            viewProj[0][3] - viewProj[0][2],
            viewProj[1][3] - viewProj[1][2],
            viewProj[2][3] - viewProj[2][2],
            viewProj[3][3] - viewProj[3][2]
        );
        
        // Normalize planes
        for (auto& plane : planes) {
            float length = glm::length(glm::vec3(plane));
            plane /= length;
        }
    }
    
    bool intersects(const BoundingBox& bbox) const {
        for (const auto& plane : planes) {
            glm::vec3 positiveVertex = bbox.min;
            
            if (plane.x >= 0) positiveVertex.x = bbox.max.x;
            if (plane.y >= 0) positiveVertex.y = bbox.max.y;
            if (plane.z >= 0) positiveVertex.z = bbox.max.z;
            
            float distance = glm::dot(glm::vec3(plane), positiveVertex) + plane.w;
            
            if (distance < 0) {
                return false;  // Outside frustum
            }
        }
        
        return true;  // Inside or intersecting frustum
    }
    
    bool intersects(const BoundingSphere& sphere) const {
        for (const auto& plane : planes) {
            float distance = glm::dot(glm::vec3(plane), sphere.center) + plane.w;
            
            if (distance < -sphere.radius) {
                return false;  // Outside frustum
            }
        }
        
        return true;
    }
};

// Uso no render loop:
void renderScene(const Scene& scene, const Camera& camera) {
    Frustum frustum;
    frustum.update(camera.getProjectionMatrix() * camera.getViewMatrix());
    
    int culledCount = 0;
    int drawnCount = 0;
    
    scene.traverse([&](SceneNode* node) {
        if (node->renderable) {
            BoundingBox worldBBox = node->renderable->bounds.transform(
                node->getWorldTransform()
            );
            
            if (frustum.intersects(worldBBox)) {
                drawNode(node);
                drawnCount++;
            } else {
                culledCount++;
            }
        }
    });
    
    // Stats
    std::cout << "Drawn: " << drawnCount << " | Culled: " << culledCount << std::endl;
}
```

### LOD System

```cpp
class LODManager {
public:
    struct LODLevel {
        Mesh* mesh;
        float distance;  // Distância máxima para este LOD
        float screenCoverage;  // % da tela (alternativa)
    };
    
    struct LODGroup {
        std::vector<LODLevel> levels;
        BoundingSphere bounds;
    };
    
    void addLODGroup(const std::string& name, const LODGroup& group) {
        lodGroups[name] = group;
    }
    
    Mesh* selectLOD(const std::string& name, 
                    const glm::vec3& objectPos,
                    const Camera& camera) const 
    {
        auto it = lodGroups.find(name);
        if (it == lodGroups.end()) return nullptr;
        
        const LODGroup& group = it->second;
        float distance = glm::distance(objectPos, camera.getPosition());
        
        // Selecionar LOD baseado em distância
        for (size_t i = 0; i < group.levels.size(); ++i) {
            if (distance <= group.levels[i].distance) {
                return group.levels[i].mesh;
            }
        }
        
        // Se passou de todas as distâncias, usar o LOD mais baixo
        return group.levels.back().mesh;
    }
    
    // Alternativa: selecionar por screen coverage
    Mesh* selectLODByScreenCoverage(const std::string& name,
                                     const glm::vec3& objectPos,
                                     const Camera& camera,
                                     const glm::vec2& screenSize) const
    {
        auto it = lodGroups.find(name);
        if (it == lodGroups.end()) return nullptr;
        
        const LODGroup& group = it->second;
        
        // Calcular coverage aproximado
        float distance = glm::distance(objectPos, camera.getPosition());
        float projectedRadius = group.bounds.radius / distance;
        float screenCoverage = (projectedRadius / screenSize.x) * 100.0f;
        
        for (size_t i = 0; i < group.levels.size(); ++i) {
            if (screenCoverage >= group.levels[i].screenCoverage) {
                return group.levels[i].mesh;
            }
        }
        
        return group.levels.back().mesh;
    }
    
private:
    std::unordered_map<std::string, LODGroup> lodGroups;
};

// Configuração de LODs:
LODManager lodManager;

LODManager::LODGroup carLODs;
carLODs.bounds = {glm::vec3(0), 5.0f};
carLODs.levels = {
    {highDetailMesh, 50.0f, 10.0f},   // LOD0: < 50m, > 10% tela
    {mediumDetailMesh, 100.0f, 5.0f}, // LOD1: < 100m, > 5% tela
    {lowDetailMesh, 200.0f, 2.0f},    // LOD2: < 200m, > 2% tela
    {veryLowDetailMesh, FLT_MAX, 0.0f}// LOD3: sempre visível
};

lodManager.addLODGroup("car", carLODs);

// No render:
Mesh* selectedMesh = lodManager.selectLOD("car", carPosition, camera);
drawMesh(selectedMesh);
```

### Occlusion Culling (GPU-based)

```cpp
// Compute shader para occlusion culling
// occlusion_cull.comp
#version 450

layout(local_size_x = 256) in;

struct ObjectData {
    mat4 transform;
    vec4 boundingSphere;  // xyz = center, w = radius
    uint visible;         // 0 = culled, 1 = visible
};

layout(set = 0, binding = 0) buffer Objects {
    ObjectData objects[];
};

layout(set = 0, binding = 1) uniform sampler2D depthPyramid;

layout(push_constant) uniform PushConstants {
    mat4 viewProj;
    vec2 screenSize;
    uint objectCount;
} pc;

bool isOccluded(vec3 center, float radius) {
    // Project sphere to screen space
    vec4 projCenter = pc.viewProj * vec4(center, 1.0);
    projCenter.xyz /= projCenter.w;
    
    // Convert to UV coordinates
    vec2 uv = projCenter.xy * 0.5 + 0.5;
    
    // Out of screen?
    if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1) {
        return false;
    }
    
    // Calculate screen-space radius
    float projRadius = radius / projCenter.w;
    
    // Select mip level based on size
    float mipLevel = log2(projRadius * max(pc.screenSize.x, pc.screenSize.y));
    mipLevel = clamp(mipLevel, 0.0, textureQueryLevels(depthPyramid) - 1.0);
    
    // Sample depth
    float sampledDepth = textureLod(depthPyramid, uv, mipLevel).r;
    
    // Compare with object depth
    return projCenter.z > sampledDepth;
}

void main() {
    uint index = gl_GlobalInvocationID.x;
    if (index >= pc.objectCount) return;
    
    ObjectData obj = objects[index];
    
    vec3 worldCenter = (obj.transform * vec4(obj.boundingSphere.xyz, 1.0)).xyz;
    float worldRadius = obj.boundingSphere.w * length(obj.transform[0].xyz);
    
    // Frustum culling primeiro
    vec4 clipPos = pc.viewProj * vec4(worldCenter, 1.0);
    if (clipPos.w < 0 || 
        abs(clipPos.x) > clipPos.w + worldRadius ||
        abs(clipPos.y) > clipPos.w + worldRadius ||
        clipPos.z < -worldRadius) {
        objects[index].visible = 0;
        return;
    }
    
    // Occlusion culling
    if (isOccluded(worldCenter, worldRadius)) {
        objects[index].visible = 0;
    } else {
        objects[index].visible = 1;
    }
}
```

```cpp
// C++ side
class OcclusionCuller {
public:
    void initialize(VkDevice device, VkExtent2D screenSize) {
        // Criar depth pyramid (mipmaps do depth buffer)
        createDepthPyramid(screenSize);
        
        // Criar compute pipeline para culling
        createCullingPipeline();
        
        // Buffer de objetos (GPU visible)
        createObjectBuffer(MAX_OBJECTS);
    }
    
    void cullObjects(VkCommandBuffer cmd, 
                     const std::vector<ObjectData>& objects,
                     const Camera& camera) 
    {
        // 1. Gerar depth pyramid
        generateDepthPyramid(cmd);
        
        // 2. Upload object data
        updateObjectBuffer(cmd, objects);
        
        // 3. Dispatch compute shader
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, cullingPipeline);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, ...);
        
        PushConstants pc;
        pc.viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();
        pc.screenSize = screenSize;
        pc.objectCount = objects.size();
        
        vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 
                          0, sizeof(pc), &pc);
        
        uint32_t groupCount = (objects.size() + 255) / 256;
        vkCmdDispatch(cmd, groupCount, 1, 1);
        
        // 4. Barrier
        VkBufferMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        barrier.buffer = objectBuffer;
        
        vkCmdPipelineBarrier(cmd,
                            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                            VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
                            0, 0, nullptr, 1, &barrier, 0, nullptr);
    }
    
    void generateDepthPyramid(VkCommandBuffer cmd) {
        // Gerar mipmaps do depth buffer usando compute shader
        for (uint32_t mip = 1; mip < mipLevels; ++mip) {
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, 
                            depthPyramidPipeline);
            
            // Bind source mip (mip - 1) e dest mip
            // ...
            
            uint32_t width = std::max(1u, screenSize.width >> mip);
            uint32_t height = std::max(1u, screenSize.height >> mip);
            
            vkCmdDispatch(cmd, (width + 7) / 8, (height + 7) / 8, 1);
            
            // Barrier entre mips
            // ...
        }
    }
};
```

### Indirect Drawing com GPU Culling

```cpp
// Após GPU culling, gerar indirect draw commands
struct DrawCommand {
    VkDrawIndexedIndirectCommand indirect;
    uint32_t objectID;
};

// Compute shader para compactar comandos visíveis
// compact_commands.comp
#version 450

layout(local_size_x = 256) in;

struct ObjectData {
    mat4 transform;
    vec4 boundingSphere;
    uint visible;
    uint meshIndex;
    uint materialIndex;
};

struct DrawCommand {
    uint indexCount;
    uint instanceCount;
    uint firstIndex;
    int  vertexOffset;
    uint firstInstance;
    uint objectID;
};

layout(set = 0, binding = 0) buffer Objects {
    ObjectData objects[];
};

layout(set = 0, binding = 1) buffer DrawCommands {
    DrawCommand commands[];
};

layout(set = 0, binding = 2) buffer DrawCount {
    uint count;
};

shared uint sharedCount;

void main() {
    uint index = gl_GlobalInvocationID.x;
    
    if (gl_LocalInvocationID.x == 0) {
        sharedCount = 0;
    }
    barrier();
    
    if (index < objects.length() && objects[index].visible == 1) {
        uint commandIndex = atomicAdd(sharedCount, 1);
        
        // Preencher comando
        DrawCommand cmd;
        cmd.indexCount = meshes[objects[index].meshIndex].indexCount;
        cmd.instanceCount = 1;
        cmd.firstIndex = meshes[objects[index].meshIndex].firstIndex;
        cmd.vertexOffset = meshes[objects[index].meshIndex].vertexOffset;
        cmd.firstInstance = 0;
        cmd.objectID = index;
        
        commands[commandIndex] = cmd;
    }
    
    barrier();
    
    if (gl_LocalInvocationID.x == 0 && sharedCount > 0) {
        atomicAdd(count, sharedCount);
    }
}
```

---

## 🔥 APÊNDICE E: SHADER PATTERNS E TÉCNICAS

### Uber Shader Pattern

```glsl
// uber_shader.frag
#version 450

// Feature flags (via specialization constants ou defines)
layout(constant_id = 0) const bool USE_NORMAL_MAP = false;
layout(constant_id = 1) const bool USE_SPECULAR_MAP = false;
layout(constant_id = 2) const bool USE_EMISSIVE_MAP = false;
layout(constant_id = 3) const bool USE_AO_MAP = false;

layout(set = 1, binding = 0) uniform sampler2D albedoMap;
layout(set = 1, binding = 1) uniform sampler2D normalMap;
layout(set = 1, binding = 2) uniform sampler2D specularMap;
layout(set = 1, binding = 3) uniform sampler2D emissiveMap;
layout(set = 1, binding = 4) uniform sampler2D aoMap;

layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in mat3 TBN;  // Tangent-Bitangent-Normal

layout(location = 0) out vec4 outColor;

vec3 getNormal() {
    if (USE_NORMAL_MAP) {
        vec3 normal = texture(normalMap, fragTexCoord).xyz;
        normal = normal * 2.0 - 1.0;  // [0,1] -> [-1,1]
        return normalize(TBN * normal);
    } else {
        return normalize(fragNormal);
    }
}

float getAO() {
    if (USE_AO_MAP) {
        return texture(aoMap, fragTexCoord).r;
    } else {
        return 1.0;
    }
}

vec3 getEmissive() {
    if (USE_EMISSIVE_MAP) {
        return texture(emissiveMap, fragTexCoord).rgb;
    } else {
        return vec3(0.0);
    }
}

void main() {
    vec3 albedo = texture(albedoMap, fragTexCoord).rgb;
    vec3 normal = getNormal();
    float ao = getAO();
    vec3 emissive = getEmissive();
    
    // Lighting calculations...
    vec3 lighting = calculateLighting(fragPos, normal, albedo);
    
    outColor = vec4(lighting * ao + emissive, 1.0);
}
```

```cpp
// Criar variantes do shader com specialization constants
VkSpecializationMapEntry entries[4];
entries[0] = {0, 0 * sizeof(uint32_t), sizeof(uint32_t)};  // USE_NORMAL_MAP
entries[1] = {1, 1 * sizeof(uint32_t), sizeof(uint32_t)};  // USE_SPECULAR_MAP
entries[2] = {2, 2 * sizeof(uint32_t), sizeof(uint32_t)};  // USE_EMISSIVE_MAP
entries[3] = {3, 3 * sizeof(uint32_t), sizeof(uint32_t)};  // USE_AO_MAP

uint32_t specializationData[4] = {1, 0, 1, 0};  // Normal=ON, Specular=OFF, Emissive=ON, AO=OFF

VkSpecializationInfo specializationInfo = {};
specializationInfo.mapEntryCount = 4;
specializationInfo.pMapEntries = entries;
specializationInfo.dataSize = sizeof(specializationData);
specializationInfo.pData = specializationData;

VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
fragShaderStageInfo.pSpecializationInfo = &specializationInfo;
```

### Clustered Deferred Shading

```glsl
// light_culling.comp
#version 450

#define TILE_SIZE 16
#define MAX_LIGHTS_PER_CLUSTER 256

layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE) in;

struct PointLight {
    vec3 position;
    float radius;
    vec3 color;
    float intensity;
};

layout(set = 0, binding = 0) uniform LightsUBO {
    PointLight lights[1024];
    uint lightCount;
};

layout(set = 0, binding = 1, r32ui) uniform uimage2D lightGrid;
layout(set = 0, binding = 2) buffer LightIndices {
    uint indices[];
};

layout(set = 0, binding = 3) uniform sampler2D depthTexture;

layout(push_constant) uniform PushConstants {
    mat4 invViewProj;
    vec2 screenSize;
};

shared uint sharedLightCount;
shared uint sharedLightIndices[MAX_LIGHTS_PER_CLUSTER];

vec3 screenToWorld(vec2 screenPos, float depth) {
    vec4 clipPos = vec4(screenPos * 2.0 - 1.0, depth, 1.0);
    vec4 worldPos = invViewProj * clipPos;
    return worldPos.xyz / worldPos.w;
}

void main() {
    ivec2 tileID = ivec2(gl_WorkGroupID.xy);
    ivec2 pixelID = ivec2(gl_GlobalInvocationID.xy);
    
    if (gl_LocalInvocationIndex == 0) {
        sharedLightCount = 0;
    }
    barrier();
    
    // Calcular frustum do tile
    vec2 tileMin = vec2(tileID) * TILE_SIZE / screenSize;
    vec2 tileMax = vec2(tileID + 1) * TILE_SIZE / screenSize;
    
    float minDepth = 1.0;
    float maxDepth = 0.0;
    
    // Encontrar min/max depth no tile
    for (int y = 0; y < TILE_SIZE; ++y) {
        for (int x = 0; x < TILE_SIZE; ++x) {
            ivec2 samplePos = tileID * TILE_SIZE + ivec2(x, y);
            if (samplePos.x < screenSize.x && samplePos.y < screenSize.y) {
                float depth = texelFetch(depthTexture, samplePos, 0).r;
                minDepth = min(minDepth, depth);
                maxDepth = max(maxDepth, depth);
            }
        }
    }
    
    // Culling de luzes por tile
    for (uint i = gl_LocalInvocationIndex; i < lightCount; i += TILE_SIZE * TILE_SIZE) {
        PointLight light = lights[i];
        
        // Project light sphere to screen space
        // ... frustum intersection test ...
        
        bool intersects = testSphereVsTileFrustum(light, tileMin, tileMax, minDepth, maxDepth);
        
        if (intersects) {
            uint index = atomicAdd(sharedLightCount, 1);
            if (index < MAX_LIGHTS_PER_CLUSTER) {
                sharedLightIndices[index] = i;
            }
        }
    }
    
    barrier();
    
    // Thread 0 escreve resultados
    if (gl_LocalInvocationIndex == 0) {
        uint startOffset = atomicAdd(globalLightIndexCount, sharedLightCount);
        imageStore(lightGrid, tileID, uvec4(startOffset, sharedLightCount, 0, 0));
        
        for (uint i = 0; i < sharedLightCount; ++i) {
            indices[startOffset + i] = sharedLightIndices[i];
        }
    }
}
```

---

**🏁 FIM DO ROADMAP COMPLETO! BOA SORTE NO DESENVOLVIMENTO DA SUA ENGINE! 🏎️💨**
