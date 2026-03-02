# 🏎️ Speed Racer Engine — Roadmap Definitivo v4.0

> **Este é o seu maior projeto pessoal. Cada linha de código importa.**  
> Construído sobre fundações sólidas (~3000 linhas) para chegar a um jogo de carro real em Vulkan.

---

## 🧭 Visão do Projeto

```
╔══════════════════════════════════════════════════════════════════╗
║                    🏁 OBJETIVO FINAL                            ║
║                                                                  ║
║   Um jogo de carro arcade rodando em Vulkan puro:               ║
║   → Modelo 3D de carro carregado do disco                       ║
║   → Câmera terceira pessoa seguindo o carro                     ║
║   → Input (WASD) com física arcade (aceleração, curvas, atrito) ║
║   → Pista 3D com colisão básica                                 ║
║   → Iluminação Phong/PBR                                        ║
║   → 60 FPS estável, zero memory leaks                           ║
╚══════════════════════════════════════════════════════════════════╝
```

### Filosofia de desenvolvimento

- **Incremental:** Uma feature por vez, nunca pular etapas
- **Testável:** Cada fase tem testes verificáveis antes de avançar
- **Entendível:** Você entende cada linha — sem copy-paste cego
- **Profissional:** Código que você vai se orgulhar de mostrar

---

## 📊 Estado Atual (Fevereiro 2026)

```
INFRAESTRUTURA VULKAN    ████████████████████░  90%  ✅ Sólida
MEMORY MANAGEMENT (VMA)  ███████████████████░░  85%  ✅ Funcional
PIPELINE / SHADERS       ██████████████░░░░░░░  65%  ⚠️  Vertex input faltando
GEOMETRIA REAL           ████░░░░░░░░░░░░░░░░░  20%  🔧 Buffer criado, não conectado
CÂMERA / UBO             ░░░░░░░░░░░░░░░░░░░░░   0%  ❌ Não iniciado
TEXTURAS                 ░░░░░░░░░░░░░░░░░░░░░   0%  ❌ Não iniciado
FÍSICA DE CARRO          ░░░░░░░░░░░░░░░░░░░░░   0%  ❌ Não iniciado
ILUMINAÇÃO               ░░░░░░░░░░░░░░░░░░░░░   0%  ❌ Não iniciado
```

### ⚡ Bug mais importante agora

```cpp
// Em createTriangle(): vertex buffer criado e enviado para GPU ✅
vertexBuffer = bufferManager->createVertexBuffer(vertices.data(), ...);

// Em recordCommandBuffer(): o buffer NUNCA É PASSADO para o draw ❌
vkCmdDraw(commandBuffer, 3, 1, 0, 0); // ainda usa dados do shader!

// SOLUÇÃO: adicionar antes do vkCmdDraw:
VkBuffer buffers[] = { bufferManager->getVkBuffer(vertexBuffer) };
VkDeviceSize offsets[] = { 0 };
vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
```

---

## 🗺️ Mapa Completo das Fases

```
FASE 0      FASE 1       FASE 2      FASE 3      FASE 4
Re-entry ──► Fix Buffer ──► Modelos ──► Câmera ──► Física
  (1d)        (2-3d)       (4-5d)      (3-4d)     (5-7d)

FASE 5      FASE 6       FASE 7      FASE 8      FASE 9+
Pista ──► Iluminação ──► Texturas ──► UI/HUD ──► Polish/PBR
 (4d)       (5-6d)        (4-5d)      (3d)       (ongoing)
```

---

## 📚 Referências de Estudo (Gerais — leia conforme avança)

### Para Vulkan

| Recurso | Tipo | Quando usar |
|---|---|---|
| [vulkan-tutorial.com](https://vulkan-tutorial.com) | Tutorial | **Referência principal** — sempre |
| [vkguide.dev](https://vkguide.dev) | Tutorial moderno | Alternativa com VMA integrado |
| [Brendan Galea — YouTube](https://www.youtube.com/c/BrendanGalea) | Vídeo | Arquitetura de engine Vulkan |
| [Sascha Willems Examples](https://github.com/SaschaWillems/Vulkan) | Código | Referência de features específicas |
| [Vulkan Spec](https://registry.khronos.org/vulkan/specs/1.3/html/) | Spec | Quando algo não funciona como esperado |

### Para Gráficos em Geral

| Recurso | Tipo | Quando usar |
|---|---|---|
| [learnopengl.com](https://learnopengl.com) | Tutorial | Conceitos transferíveis (MVP, lighting, textures) |
| [Real-Time Rendering](https://www.realtimerendering.com/) | Livro | Referência de rendering avançado |
| [3D Math Primer for Graphics](https://gamemath.com/) | Livro (grátis) | Matemática de transformações, câmera |
| [Physically Based Rendering](https://pbr-book.org/) | Livro (grátis) | PBR quando chegar lá |

### Para C++ Moderno

| Recurso | Tipo | Foco |
|---|---|---|
| [cppreference.com](https://en.cppreference.com) | Referência | Consulta constante |
| [Effective Modern C++](https://www.oreilly.com/library/view/effective-modern-c/9781491908419/) | Livro | Smart pointers, move semantics |
| [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/) | Guia | Boas práticas |

### Para Ferramentas

| Ferramenta | Link | Para que serve |
|---|---|---|
| **RenderDoc** | [renderdoc.org](https://renderdoc.org/) | Debug visual de frames Vulkan — **use sempre** |
| **Valgrind** | Sistema | Detectar memory leaks |
| **glm** | já no projeto | Matemática (vetores, matrizes) |
| **tracy** | [github](https://github.com/wolfpld/tracy) | Profiling CPU/GPU |

---

## 🏁 FASE 0 — Re-entry (1–2 dias)

> **Objetivo:** Voltar a entender o código sem escrever nada novo. Mapa mental antes de qualquer implementação.

### Tarefas

```
[ ] Rodar: tools/build_and_run.sh → confirmar triângulo na tela
[ ] Compilar shaders manualmente:
      mkdir -p assets/shaders/core/compiled
      glslc assets/shaders/core/basic.vert -o assets/shaders/core/compiled/vert.spv
      glslc assets/shaders/core/basic.frag -o assets/shaders/core/compiled/frag.spv
[ ] Ler VulkanManager.hpp completo (5 min)
[ ] Rastrear fluxo: main() → run() → initVulkan() → mainLoop() → drawFrame()
[ ] Identificar o gap: createTriangle() vs recordCommandBuffer()
[ ] Abrir RenderDoc, capturar um frame, ver os draw calls
[ ] Reler plan.md e riscas o que já foi feito
```

### O que estudar nessa fase

- Não estude nada novo — **foque em entender o que já existe**
- Se sentir perdido: releia os capítulos "Drawing a triangle" do vulkan-tutorial.com

### Checklist de saúde do projeto

```
[ ] Compila sem warnings?
[ ] Triângulo aparece na tela?
[ ] Validation layers sem erros?
[ ] Resize da janela funciona?
[ ] Fechar a janela não crasha?
```

---

## 🔧 FASE 1 — Conectar o Vertex Buffer (2–3 dias)

> **Objetivo:** O triângulo usa dados reais da GPU. Primeira vez que C++ controla a geometria.
>
> **Resultado visível:** Mesma imagem, mas agora você pode mudar as posições e cores em C++ e ver na tela.

### Conceitos a aprender nessa fase

| Conceito | Onde estudar | Por que importa |
|---|---|---|
| Vertex Input Description | [vulkan-tutorial.com/Vertex-buffers/Vertex-input-description](https://vulkan-tutorial.com/Vertex_buffers/Vertex_input_description) | Diz para o Vulkan como interpretar seus dados |
| VkVertexInputBindingDescription | Spec / tutorial acima | Descreve o stride do buffer |
| VkVertexInputAttributeDescription | Spec / tutorial acima | Descreve cada atributo (pos, color, uv...) |
| vkCmdBindVertexBuffers | [Capítulo vertex buffers](https://vulkan-tutorial.com/Vertex_buffers/Vertex_buffer_creation) | Conecta o buffer ao pipeline antes do draw |

### O que implementar

**Passo 1 — Adicionar métodos ao `struct Vertex` (VulkanManager.hpp)**

```cpp
struct Vertex {
    float pos[3];
    float color[3];

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription binding{};
        binding.binding   = 0;
        binding.stride    = sizeof(Vertex);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attrs{};
        // posição: vec3 = 3 floats
        attrs[0].binding  = 0;
        attrs[0].location = 0;
        attrs[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attrs[0].offset   = offsetof(Vertex, pos);
        // cor: vec3 = 3 floats
        attrs[1].binding  = 0;
        attrs[1].location = 1;
        attrs[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attrs[1].offset   = offsetof(Vertex, color);
        return attrs;
    }
};
```

**Passo 2 — Atualizar `PipelineManager.cpp`**

```cpp
// Substituir vertexInputInfo com count = 0:
auto bindingDesc = Vertex::getBindingDescription();
auto attrDescs   = Vertex::getAttributeDescriptions();

VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
vertexInputInfo.vertexBindingDescriptionCount   = 1;
vertexInputInfo.pVertexBindingDescriptions      = &bindingDesc;
vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attrDescs.size());
vertexInputInfo.pVertexAttributeDescriptions    = attrDescs.data();
```

**Passo 3 — Atualizar `recordCommandBuffer()`**

```cpp
// Adicionar ANTES de vkCmdDraw:
VkBuffer vertexBuffers[] = { bufferManager->getVkBuffer(vertexBuffer) };
VkDeviceSize offsets[]   = { 0 };
vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
```

**Passo 4 — Atualizar os shaders**

```glsl
// basic.vert — substituir posições hardcoded:
#version 450
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(inPos, 1.0);
    fragColor = inColor;
}
```

### Testes da Fase 1

```
[ ] Triângulo aparece com cores corretas (vermelho, verde, azul)
[ ] Mudar posições em createTriangle() → triângulo muda na tela
[ ] Mudar cores em createTriangle() → cores mudam na tela
[ ] Validation layers: zero errors, zero warnings
[ ] Resize continua funcionando
[ ] RenderDoc: vertex buffer aparece nos resources do draw call
```

---

## 📦 FASE 2 — Index Buffer + Mesh System (3–4 dias)

> **Objetivo:** Criar um sistema de Mesh reutilizável. Renderizar quadrado e cubo.
>
> **Resultado visível:** Cubo wireframe ou sólido na tela, formado por faces indexadas.

### Conceitos a aprender

| Conceito | Onde estudar | Por que importa |
|---|---|---|
| Index Buffer | [vulkan-tutorial.com — Index buffer](https://vulkan-tutorial.com/Vertex_buffers/Index_buffer) | Reutilizar vértices (cubo: 8v em vez de 36) |
| vkCmdDrawIndexed | Tutorial acima | Draw com índices |
| Mesh como abstração | Qualquer livro de game engine | Separação de dados de geometria |

### O que implementar

**Struct Mesh (novo arquivo `include/rendering/Mesh.hpp`)**

```cpp
#pragma once
#include <core/BufferManager.hpp>
#include <vector>

struct MeshData {
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
};

struct Mesh {
    BufferHandle vertexBuffer = INVALID_HANDLE;
    BufferHandle indexBuffer  = INVALID_HANDLE;
    uint32_t     indexCount   = 0;

    // Factory methods
    static MeshData makeTriangle();
    static MeshData makeQuad();
    static MeshData makeCube();
};
```

**Helpers de geometria (`src/rendering/Mesh.cpp`)**

```cpp
MeshData Mesh::makeQuad() {
    return {
        // Vértices (posição, cor)
        { {{-0.5f,-0.5f,0}, {1,0,0}},
          {{ 0.5f,-0.5f,0}, {0,1,0}},
          {{ 0.5f, 0.5f,0}, {0,0,1}},
          {{-0.5f, 0.5f,0}, {1,1,0}} },
        // Índices (2 triângulos)
        { 0, 1, 2, 2, 3, 0 }
    };
}
```

**Atualizar `recordCommandBuffer()`**

```cpp
// Bind vertex buffer
VkBuffer vb[] = { bufferManager->getVkBuffer(mesh.vertexBuffer) };
VkDeviceSize offsets[] = { 0 };
vkCmdBindVertexBuffers(commandBuffer, 0, 1, vb, offsets);

// Bind index buffer
vkCmdBindIndexBuffer(commandBuffer,
    bufferManager->getVkBuffer(mesh.indexBuffer), 0, VK_INDEX_TYPE_UINT32);

// Draw indexed
vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, 0, 0, 0);
```

### Testes da Fase 2

```
[ ] Quad (2 triângulos) renderiza corretamente
[ ] Cubo renderiza (8 vértices, 36 índices)
[ ] Mudar vértices de um quad → shape muda na tela
[ ] Index count errado → validation layer avisa
[ ] Criar e destruir 100 meshes sem memory leaks
[ ] RenderDoc: index buffer visível nos resources
```

---

## 🚗 FASE 3 — Carregar Modelo 3D do Carro (4–5 dias)

> **Objetivo:** Substituir o cubo por um modelo .obj/.gltf carregado do disco via Assimp.
>
> **Resultado visível:** Modelo 3D de carro na tela, estático.

### Conceitos a aprender

| Conceito | Onde estudar | Por que importa |
|---|---|---|
| Assimp basics | [Assimp docs](https://assimp-docs.readthedocs.io/) + learnopengl.com/Model-Loading/Assimp | Parsing de formatos 3D |
| Scene graph Assimp | Docs acima | aiScene, aiNode, aiMesh |
| Múltiplas meshes | learnopengl.com/Model-Loading/Model | Um modelo pode ter N meshes |
| Asset paths | std::filesystem docs | Caminhos portáveis |

### De onde pegar o modelo de carro

```
Opções gratuitas:
  → Sketchfab.com (filtrar: free, low-poly, .glb/.gltf)
  → Kenney.nl (assets grátis, low-poly perfeito pra começar)
  → TurboSquid (filtrar free)

Formato recomendado: .gltf ou .obj (ambos suportados pelo Assimp)
Poly count recomendado: 500–5000 polígonos pra começar
```

### O que implementar

**ModelLoader (`include/rendering/ModelLoader.hpp`)**

```cpp
#pragma once
#include <rendering/Mesh.hpp>
#include <core/BufferManager.hpp>
#include <string>
#include <vector>

struct Model {
    std::vector<Mesh> meshes;
    std::string name;
};

class ModelLoader {
public:
    ModelLoader(BufferManager& buffers);
    Model load(const std::string& path);
private:
    BufferManager& buffers;
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};
```

**AssetManager simples (`include/core/AssetManager.hpp`)**

```cpp
#pragma once
#include <filesystem>
#include <string>

class AssetManager {
public:
    // Configura diretório base na inicialização
    static void setBasePath(const std::filesystem::path& base);
    static std::string model(const std::string& filename);
    static std::string shader(const std::string& filename);
    static std::string texture(const std::string& filename);
private:
    static std::filesystem::path basePath;
};

// Uso: AssetManager::model("car.gltf") → "/projeto/assets/models/car.gltf"
```

### Integrar no projeto

```cpp
// Em VulkanManager::initVulkan(), após setupVmaWrapper():
AssetManager::setBasePath(std::filesystem::current_path().parent_path());
model = modelLoader->load(AssetManager::model("car.gltf"));
```

### Testes da Fase 3

```
[ ] Modelo carrega sem crash
[ ] Modelo aparece na tela (pode estar errado de ângulo/escala — normal)
[ ] Modelo inválido/inexistente → exception com mensagem clara
[ ] Múltiplas meshes do modelo renderizando todas
[ ] Vertex count correto (compare com o que o Blender mostra)
[ ] Memory: destruir modelo → sem leaks
[ ] RenderDoc: vertices do carro visíveis no frame
```

---

## 📷 FASE 4 — UBOs, Transforms e Câmera (4–5 dias)

> **Objetivo:** Mover e orientar objetos via matrizes. Câmera 3D navegável.
>
> **Resultado visível:** Câmera orbitando o carro. Carro pode ser posicionado no espaço.

### Conceitos a aprender

| Conceito | Onde estudar | Por que importa |
|---|---|---|
| Matrizes MVP | [learnopengl.com/Getting-started/Transformations](https://learnopengl.com/Getting-started/Transformations) | Fundação de todo rendering 3D |
| Uniform Buffers | [vulkan-tutorial.com/Uniform-buffers](https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer) | Passar dados por frame pro shader |
| Descriptor Sets | Capítulo acima | Como o shader recebe dados dinâmicos |
| GLM (biblioteca) | [glm.g-truc.net](https://glm.g-truc.net/0.9.9/api/a00206.html) | Matemática: vec3, mat4, lookAt, perspective |
| Câmera Look-At | [learnopengl.com/Getting-started/Camera](https://learnopengl.com/Getting-started/Camera) | View matrix |

### Matemática que você precisa entender (invista tempo aqui)

```
Model Matrix:    Onde está o objeto no mundo (posição, rotação, escala)
View Matrix:     Onde está a câmera (glm::lookAt)
Projection:      Como projetar 3D em 2D (glm::perspective)
MVP = Proj * View * Model → gl_Position = MVP * vertex
```

**Recurso extra:** [3D Math Primer](https://gamemath.com/) — capítulos 4 (vetores), 5 (matrizes), 10 (viewing)

### O que implementar

**UBO struct**

```cpp
#include <glm/glm.hpp>

struct UniformBufferObject {
    glm::mat4 model;  // Transform do objeto
    glm::mat4 view;   // Câmera
    glm::mat4 proj;   // Perspectiva
};
```

**DescriptorManager (novo)**

```cpp
class DescriptorManager {
public:
    DescriptorManager(VkDevice device, uint32_t framesInFlight);
    ~DescriptorManager();

    VkDescriptorSetLayout getLayout() const;
    VkDescriptorSet       getSet(uint32_t frame) const;

    void updateUBO(uint32_t frame, const UniformBufferObject& ubo,
                   BufferManager& buffers);
private:
    VkDevice              device;
    VkDescriptorPool      pool;
    VkDescriptorSetLayout layout;
    std::vector<VkDescriptorSet>  sets;
    std::vector<BufferHandle>     uboBuffers;
};
```

**Câmera**

```cpp
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 position = {0, 3, -8};  // Atrás e acima do carro
    glm::vec3 target   = {0, 0, 0};

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, target, glm::vec3(0,1,0));
    }
    glm::mat4 getProjectionMatrix(float aspect) const {
        auto proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
        proj[1][1] *= -1; // Vulkan Y invertido vs OpenGL
        return proj;
    }
};
```

**Atualizar shaders**

```glsl
// basic.vert
layout(set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPos, 1.0);
    fragColor = inColor;
}
```

### Testes da Fase 4

```
[ ] Carro aparece no centro da tela (model = identity)
[ ] Alterar posição da câmera → perspectiva muda
[ ] Carro rotaciona quando você muda model matrix com sin(time)
[ ] Resize: aspect ratio da projeção atualiza corretamente
[ ] Múltiplos objetos com transforms independentes
[ ] Validation layers: descriptor sets sem erros
[ ] UBO atualiza a cada frame sem flickering
```

---

## ⌨️ FASE 5 — Input System (2–3 dias)

> **Objetivo:** Abstração limpa para teclado e mouse. Base para controlar o carro.
>
> **Resultado visível:** WASD move a câmera. Mouse orbita o carro.

### Conceitos a aprender

| Conceito | Onde estudar |
|---|---|
| GLFW Input | [glfw.org/docs/latest/input_guide.html](https://www.glfw.org/docs/latest/input_guide.html) |
| Delta time | [learnopengl.com — Camera](https://learnopengl.com/Getting-started/Camera) (seção "walk around") |
| Mouse callback GLFW | Docs GLFW acima |

### O que implementar

```cpp
// include/core/InputManager.hpp
class InputManager {
public:
    void initialize(GLFWwindow* window);
    void update(); // Chamar no início de cada frame

    bool isKeyDown(int key)      const;
    bool isKeyJustPressed(int key) const;  // Só no frame que pressionou
    bool isKeyJustReleased(int key) const;

    glm::vec2 getMouseDelta() const;
    glm::vec2 getMousePos()   const;

    void setCursorLocked(bool locked);
private:
    GLFWwindow* window;
    std::unordered_set<int> currentKeys;
    std::unordered_set<int> previousKeys;
    glm::vec2 mousePos, lastMousePos;
    bool firstMouse = true;
};
```

### Testes da Fase 5

```
[ ] W pressionado → isKeyDown(GLFW_KEY_W) retorna true
[ ] W liberado → isKeyDown retorna false no próximo frame
[ ] isKeyJustPressed → true apenas no primeiro frame
[ ] Mouse delta correto (não explode no primeiro frame)
[ ] ESC → libera cursor
[ ] Câmera orbita o carro com o mouse
```

---

## 🚗 FASE 6 — Física Básica do Carro (5–7 dias)

> **Objetivo:** Fazer o carro se mover com física arcade simples. Sem engine de física externa.
>
> **Resultado visível:** WASD controla o carro. Câmera segue. Aceleração, curvas, atrito.

### Conceitos a aprender

| Conceito | Onde estudar | Por que importa |
|---|---|---|
| Física de carro arcade | [Game Physics Cookbook](https://gdcvault.com/play/1023559/Math-for-Game-Developers-Collision) | Fundação da jogabilidade |
| Delta time e integração | learnopengl.com câmera | Frame-rate independence |
| Quaternions (rotação) | [learnopengl — transformations](https://learnopengl.com/Getting-started/Transformations) + glm::quat | Rotação suave sem gimbal lock |
| Forward vector | 3D Math Primer cap. 4 | Saber para onde o carro aponta |

### Modelo de física arcade (simples e funcional)

```
Não é física realista — é diversão:
  → Aceleração: velocidade aumenta quando W pressionado
  → Direção:    A/D roda o carro, que afeta a direção do movimento
  → Atrito:     Velocidade diminui naturalmente (multiplica por 0.95/frame)
  → Max speed:  Limite de velocidade máxima
```

### O que implementar

```cpp
// include/game/Car.hpp
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Car {
public:
    // Estado
    glm::vec3 position    = {0, 0, 0};
    glm::quat rotation    = glm::quat(1, 0, 0, 0);
    float     speed       = 0.0f;

    // Parâmetros (tuning)
    float maxSpeed        = 20.0f;
    float acceleration    = 15.0f;
    float brakeForce      = 20.0f;
    float turnSpeed       = 2.5f;
    float friction        = 0.92f;  // Fator de desaceleração (0-1)

    void update(float dt, bool accel, bool brake, float steer);
    glm::mat4 getModelMatrix() const;
    glm::vec3 getForward()     const;

private:
    float currentSteer = 0.0f;
};
```

```cpp
// src/game/Car.cpp
void Car::update(float dt, bool accel, bool brake, float steer) {
    // Direção (só quando está se movendo)
    if (std::abs(speed) > 0.1f) {
        float steerAmount = steer * turnSpeed * dt * (speed / maxSpeed);
        glm::quat rot = glm::angleAxis(steerAmount, glm::vec3(0,1,0));
        rotation = glm::normalize(rot * rotation);
    }

    // Aceleração
    if (accel)  speed += acceleration * dt;
    if (brake)  speed -= brakeForce   * dt;

    // Atrito
    speed *= std::pow(friction, dt * 60.0f);

    // Clamp
    speed = glm::clamp(speed, -maxSpeed * 0.5f, maxSpeed);

    // Mover na direção que o carro aponta
    position += getForward() * speed * dt;
}

glm::vec3 Car::getForward() const {
    return rotation * glm::vec3(0, 0, 1); // Z+ é frente
}

glm::mat4 Car::getModelMatrix() const {
    glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 r = glm::mat4_cast(rotation);
    return t * r;
}
```

**Câmera terceira pessoa seguindo o carro**

```cpp
void updateCamera(Camera& cam, const Car& car) {
    glm::vec3 camOffset = car.rotation * glm::vec3(0, 3, -8);
    cam.position = car.position + camOffset;
    cam.target   = car.position + glm::vec3(0, 1, 0);
}
```

### Loop de game completo

```cpp
// Em VulkanManager::mainLoop():
float lastTime = glfwGetTime();
while (!window.shouldClose()) {
    float now = glfwGetTime();
    float dt  = now - lastTime;
    lastTime  = now;

    input->update();

    // Input do carro
    bool accel = input->isKeyDown(GLFW_KEY_W);
    bool brake = input->isKeyDown(GLFW_KEY_S);
    float steer = 0;
    if (input->isKeyDown(GLFW_KEY_A)) steer = -1;
    if (input->isKeyDown(GLFW_KEY_D)) steer =  1;

    car->update(dt, accel, brake, steer);
    updateCamera(*camera, *car);

    // Atualizar UBO
    UniformBufferObject ubo{};
    ubo.model = car->getModelMatrix();
    ubo.view  = camera->getViewMatrix();
    ubo.proj  = camera->getProjectionMatrix(aspect);
    descriptorManager->updateUBO(currentFrame, ubo, *bufferManager);

    window.pollEvents();
    drawFrame();
}
```

### Testes da Fase 6

```
[ ] W → carro acelera para frente
[ ] S → carro freia/recua
[ ] A/D → carro vira (mais pronunciado em alta velocidade)
[ ] Soltar tudo → carro desacelera até parar
[ ] Câmera segue o carro suavemente
[ ] 60 FPS estável com o carro se movendo
[ ] Velocidade máxima respeitada
[ ] Frame-rate independent: 30fps e 144fps = mesma sensação de física
[ ] Velocidade negativa limitada (não vai tão rápido para trás)
```

---

## 🏟️ FASE 7 — Pista e Profundidade (3–4 dias)

> **Objetivo:** Chão 3D, depth buffer funcionando. Carro dirige em cima de algo.
>
> **Resultado visível:** Pista plana texturizada. Carro não passa pelo chão.

### Conceitos a aprender

| Conceito | Onde estudar |
|---|---|
| Depth Buffer | [vulkan-tutorial.com/Depth-buffering](https://vulkan-tutorial.com/Depth_buffering) |
| Z-fighting | Artigo de resolução: aumentar near plane, polygon offset |
| Múltiplas meshes na cena | Qualquer engine tutorial |

### O que implementar

**Depth buffer (crítico para 3D)**

```cpp
// Adicionar ao SwapchainManager ou VulkanManager:
// 1. Criar imagem de depth (VMA)
// 2. Atualizar render pass com depth attachment
// 3. Habilitar depth testing no pipeline:
depthStencil.depthTestEnable  = VK_TRUE;
depthStencil.depthWriteEnable = VK_TRUE;
depthStencil.depthCompareOp   = VK_COMPARE_OP_LESS;
// 4. Atualizar framebuffers para incluir depth
```

**Pista simples**

```cpp
// Quad gigante = pista plana
MeshData trackData = Mesh::makeQuad();
// Escalar para 200x200 unidades
for (auto& v : trackData.vertices) {
    v.pos[0] *= 100.0f;
    v.pos[2] *= 100.0f;
}
Mesh track = createMeshFromData(trackData, *bufferManager);
```

### Testes da Fase 7

```
[ ] Pista aparece abaixo do carro
[ ] Carro não afunda/atravessa a pista
[ ] Depth buffer: objetos distantes não aparecem na frente dos próximos
[ ] Sem z-fighting entre pista e carro
[ ] Resize: depth buffer recriado corretamente
```

---

## 💡 FASE 8 — Iluminação Básica (5–6 dias)

> **Objetivo:** Iluminação Phong. O carro parece um objeto 3D real, não uma silhueta plana.
>
> **Resultado visível:** Luz, sombra difusa, especular. Parece um jogo.

### Conceitos a aprender

| Conceito | Onde estudar | Por que importa |
|---|---|---|
| Modelo Phong | [learnopengl.com/Lighting/Basic-Lighting](https://learnopengl.com/Lighting/Basic-Lighting) | Ambient + Diffuse + Specular |
| Normais de vértice | [learnopengl.com/Lighting/Basic-Lighting](https://learnopengl.com/Lighting/Basic-Lighting) | O shader precisa da normal pra calcular luz |
| Espaço de coordenadas | [learnopengl.com/Lighting/Colors](https://learnopengl.com/Lighting/Colors) | Normal deve estar no espaço correto |
| Normal Matrix | [glm normal matrix](https://stackoverflow.com/questions/13654401) | Transformar normais corretamente |

### Atualizar Vertex para incluir normal

```cpp
struct Vertex {
    float pos[3];
    float color[3];
    float normal[3];  // NOVO

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
    // (atualizar para 3 attributos: pos, color, normal)
};
```

### UBO expandido

```cpp
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec4 lightPos;    // xyz = posição da luz, w = intensidade
    glm::vec4 lightColor;  // rgb = cor da luz
    glm::vec4 viewPos;     // xyz = posição da câmera (para specular)
};
```

### Fragment shader com Phong

```glsl
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragColor;

layout(set = 0, binding = 0) uniform UBO { ... } ubo;

void main() {
    vec3 norm     = normalize(fragNormal);
    vec3 lightDir = normalize(ubo.lightPos.xyz - fragPos);
    vec3 viewDir  = normalize(ubo.viewPos.xyz - fragPos);
    vec3 halfDir  = normalize(lightDir + viewDir);

    // Componentes
    float ambient  = 0.15;
    float diffuse  = max(dot(norm, lightDir), 0.0);
    float specular = pow(max(dot(norm, halfDir), 0.0), 32.0);

    vec3 result = (ambient + diffuse + specular * 0.5) * fragColor * ubo.lightColor.rgb;
    outColor = vec4(result, 1.0);
}
```

### Testes da Fase 8

```
[ ] Carro tem gradiente de luz (lado iluminado vs sombra)
[ ] Mover a luz → iluminação muda
[ ] Specular visível em ângulos corretos
[ ] Normais corretas no modelo (sem faces "invertidas")
[ ] Performance: ainda 60 FPS com iluminação
```

---

## 🎨 FASE 9 — Texturas (4–5 dias)

> **Objetivo:** Texturas no carro e na pista. Parece um jogo de verdade.
>
> **Resultado visível:** Carro com textura de cor/material. Pista com asfalto.

### Conceitos a aprender

| Conceito | Onde estudar |
|---|---|
| Texturas em Vulkan | [vulkan-tutorial.com/Texture-mapping](https://vulkan-tutorial.com/Texture_mapping/Images) |
| stb_image (loading) | [github.com/nothings/stb](https://github.com/nothings/stb) |
| Mipmaps | Vulkan tutorial acima |
| Sampler | Vulkan tutorial acima |
| UV Coordinates | [learnopengl.com — Textures](https://learnopengl.com/Getting-started/Textures) |

### O que implementar

**TextureManager (novo)**

```cpp
class TextureManager {
public:
    TextureManager(VkDevice, VkPhysicalDevice, VmaAllocator, CommandManager&, QueueManager&);

    struct Texture {
        VkImage       image;
        VkImageView   view;
        VkSampler     sampler;
        VmaAllocation allocation;
    };

    Texture loadTexture(const std::string& path);
    void    destroy(Texture& tex);

private:
    void createImage(uint32_t w, uint32_t h, VkFormat, VkImageUsageFlags, Texture&);
    void transitionLayout(VkImage, VkImageLayout old, VkImageLayout next);
    void copyBufferToImage(VkBuffer, VkImage, uint32_t w, uint32_t h);
    void generateMipmaps(VkImage, uint32_t w, uint32_t h);
};
```

**Descriptor set atualizado**

```
Set 0, Binding 0 → UBO (matrizes + luz)
Set 0, Binding 1 → sampler2D (textura do albedo)
```

### Testes da Fase 9

```
[ ] Carro mostra textura corretamente (sem distorção)
[ ] Pista com textura de asfalto
[ ] Textura inválida → exception com mensagem clara
[ ] Múltiplas texturas simultâneas
[ ] Mipmaps: textura muda de detalhe com distância
[ ] Memory: destruir textura libera VkImage e VmaAllocation
```

---

## 🖥️ FASE 10 — UI/HUD (2–3 dias)

> **Objetivo:** Velocímetro, painel de debug. ImGui já está no projeto!
>
> **Resultado visível:** Janela de debug com velocidade, posição do carro, FPS.

### Conceitos a aprender

| Conceito | Onde estudar |
|---|---|
| ImGui Vulkan backend | [imgui/examples/imgui_impl_vulkan.h](https://github.com/ocornut/imgui/tree/master/backends) |
| ImGui básico | [imgui README](https://github.com/ocornut/imgui) |
| Overlay vs janela | Docs ImGui |

### O que implementar

```cpp
// Adicionar ao render loop:
ImGui_ImplVulkan_NewFrame();
ImGui_ImplGlfw_NewFrame();
ImGui::NewFrame();

ImGui::SetNextWindowPos({10, 10});
ImGui::Begin("Speed Racer Debug");
ImGui::Text("Speed: %.1f km/h", car->speed * 3.6f);
ImGui::Text("Position: %.1f, %.1f", car->position.x, car->position.z);
ImGui::Text("FPS: %.0f", 1.0f/deltaTime);
ImGui::SliderFloat("Max Speed", &car->maxSpeed, 5, 50);
ImGui::SliderFloat("Acceleration", &car->acceleration, 1, 30);
ImGui::End();

ImGui::Render();
ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
```

---

## 🔮 FASE 11+ — Futuro / Bonus

> Priorize conforme sentir vontade, sem prazo

```
[ ] Shadows (shadow mapping — vulkan-tutorial não tem, use Sascha Willems examples)
[ ] PBR (Physically Based Rendering — pbr-book.org)
[ ] Som (OpenAL ou miniaudio — simples de integrar)
[ ] Múltiplos carros (ECS como planejado nos MDs antigos)
[ ] Pista com curvas (bezier ou .gltf customizado)
[ ] Colisão real (Bullet Physics ou Jolt)
[ ] Bloom / post-processing
[ ] Skybox
[ ] Profiler integrado (Tracy)
```

---

## 📋 Checklist de Qualidade (use sempre)

### Antes de commitar qualquer coisa

```
[ ] Compila sem warnings (-Wall -Wextra)?
[ ] Validation layers sem erros?
[ ] Valgrind/ASan: sem leaks no fluxo principal?
[ ] Resize da janela continua funcionando?
[ ] A feature que eu adicionei tem pelo menos 1 teste?
[ ] Comentei o "por quê", não o "o quê"?
```

### A cada fase concluída

```
[ ] RenderDoc: capturar frame e inspecionar os draw calls
[ ] Commitar com mensagem descritiva (feat: add vertex buffer, fix: shader path)
[ ] Atualizar o changelog pessoal (docs/Diary.md já existe!)
[ ] Screenshot ou vídeo curto do resultado
```

---

## 🛠️ Setup de Desenvolvimento Recomendado

### CMake com compilação automática de shaders

```cmake
# Adicionar ao CMakeLists.txt:
find_program(GLSLC glslc HINTS ENV VULKAN_SDK PATH_SUFFIXES bin)

function(compile_shader TARGET SHADER)
    get_filename_component(FNAME ${SHADER} NAME)
    set(OUTPUT "${CMAKE_BINARY_DIR}/shaders/${FNAME}.spv")
    add_custom_command(
        OUTPUT  ${OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/shaders"
        COMMAND ${GLSLC} ${SHADER} -o ${OUTPUT}
        DEPENDS ${SHADER}
    )
    target_sources(${TARGET} PRIVATE ${OUTPUT})
endfunction()

compile_shader(Speed_Racer ${CMAKE_SOURCE_DIR}/assets/shaders/core/basic.vert)
compile_shader(Speed_Racer ${CMAKE_SOURCE_DIR}/assets/shaders/core/basic.frag)
```

### GoogleTest para testes unitários

```cmake
# Adicionar ao CMakeLists.txt:
include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/v1.14.0.zip)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(Tests
    tests/test_mesh.cpp
    tests/test_car_physics.cpp
    tests/test_shader.cpp
)
target_link_libraries(Tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(Tests)
```

### Estrutura de pastas recomendada

```
Speed_Racer/
├── assets/
│   ├── models/        ← car.gltf, track.gltf
│   ├── shaders/
│   │   ├── core/      ← .vert/.frag source
│   │   └── compiled/  ← .spv (gerado automaticamente)
│   └── textures/
├── include/
│   ├── core/          ← infraestrutura Vulkan (já existe)
│   ├── rendering/     ← Mesh, ModelLoader, TextureManager, Camera
│   └── game/          ← Car, InputManager, Track
├── src/
│   ├── core/          ← implementações (já existe)
│   ├── rendering/
│   └── game/
└── tests/             ← testes GoogleTest
    ├── test_mesh.cpp
    ├── test_car.cpp
    └── test_shader.cpp
```

---

## 📅 Estimativa de Tempo

| Fase | Dias estimados | Resultado |
|---|---|---|
| 0 — Re-entry | 1–2 | Entende o código |
| 1 — Vertex Buffer | 2–3 | Triângulo controlado por C++ |
| 2 — Index + Mesh | 3–4 | Cubo na tela |
| 3 — Modelo do Carro | 4–5 | Carro 3D estático |
| 4 — UBOs + Câmera | 4–5 | Carro no espaço 3D |
| 5 — Input | 2–3 | Controles básicos |
| 6 — Física do Carro | 5–7 | **CARRO DIRIGINDO** 🎉 |
| 7 — Pista + Depth | 3–4 | Pista 3D |
| 8 — Iluminação | 5–6 | Parece um jogo |
| 9 — Texturas | 4–5 | Parece um jogo bonito |
| 10 — UI/HUD | 2–3 | Polido |
| **Total** | **~6–9 semanas** | **Protótipo jogável** |

---

## 💬 Motivação

Você já escreveu 3000+ linhas para ter um triângulo. Sabe o que isso significa?

Você tem a parte mais difícil pronta: toda a infraestrutura Vulkan (que a maioria das pessoas abandona). O que falta agora são features de alto nível que constroem umas sobre as outras, cada uma com resultado visível imediato.

**Cada fase é um checkpoint que você pode mostrar para alguém e dizer "olha o que eu fiz".**

Da para fazer o carro dirigir em umas 4–5 semanas de trabalho focado. E o código que você tem hoje é bom o suficiente para chegar lá sem reescrever nada do zero.

Vai fundo. 🏎️💨
