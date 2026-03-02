# 🏎️ Speed Racer Engine — Roadmap v5.0 (com MathVier)

> **Este roadmap assume que você vai usar sua própria biblioteca matemática.**
> Isso eleva o projeto: você não só faz um jogo em Vulkan — você faz com matemática que escreveu do zero.

---

## 📦 Estado Atual da MathVier

Antes de tudo, mapa honesto do que tem e do que falta:

```
Vec2<T>          ✅  length, normalized, dot, cross, +, -, *
Vec3<T>          ✅  length, normalized, dot, cross, +, -
                 ⚠️  Falta operator*(scalar) → necessário para física
Vec4<T>          ✅  length, normalized, dot, +, -, *
Mat4<T>          ✅  identity, scale, translate, rotate (Rodrigues)
                 ✅  perspective, lookAt, operator*, operator*(Vec4)
                 ⚠️  Falta toArray() / data() → necessário para Vulkan memcpy
Quaternion<T>    ❌  Bugs de compilação (Type vs type_t, std:: faltando)
                 ❌  Falta fromAngleAxis() → necessário para rotação do carro
                 ❌  Falta toMat4()        → necessário para render
VectorGeneric<T> ✅  push, dot, sum, pow, length, normalized
DynamicArray<T>  ✅  hold, push, free
Legado (vec_t)   ✅  Mantido por nostalgia — não usar no engine novo
```

### 🐛 Bugs confirmados no Quaternion.hpp

```cpp
// BUG 1: usa 'Type' mas o template define 'type_t'
Type length() const { ... }    // ← ERRO: Type não existe
Type len = length();           // ← ERRO: Type não existe

// BUG 2: falta std::
throw out_of_range("...");     // ← ERRO: deve ser std::out_of_range

// BUG 3: chave extra no final
};   // fecha struct
};   // fecha namespace → CORRETO
```

---

## 🗺️ Dois trilhos em paralelo

```
ENGINE (Vulkan)           MATHVIER (sua lib)
══════════════            ════════════════════
Fase 0: Re-entry    ←──── Fix Quaternion + Vec3::operator*
Fase 1: Vertex Buf  ←──── Mat4::toArray() para Vulkan
Fase 2: Mesh Sys    ←──── (usa o que tem)
Fase 3: Modelo 3D   ←──── (usa o que tem)
Fase 4: UBO/Câmera  ←──── Camera usa Mat4::lookAt + perspective
Fase 5: Input       ←──── (só GLFW, sem math)
Fase 6: Física      ←──── Quaternion::fromAngleAxis + toMat4
Fase 7: Pista/Depth ←──── (usa o que tem)
Fase 8: Iluminação  ←──── Vec3 normalização + dot no shader
Fase 9: Texturas    ←──── (sem dependência nova)
```

**Regra:** Não avance uma fase do engine sem as dependências da MathVier estarem prontas e testadas.

---

## 🔧 PRÉ-FASE — Consertar a MathVier (1–2 dias)

> **Faça isso antes de qualquer coisa.** O engine não compila sem isso.

### Fix 1 — Quaternion.hpp completo e corrigido

```cpp
// include/MathVier/Quaternion.hpp
#ifndef MATHVIER_QUATERNION_HPP
#define MATHVIER_QUATERNION_HPP

#include <MathVier/vector.hpp>
#include <stdexcept>
#include <cmath>

namespace MathVier {

template<typename type_t>
struct alignas(16) Quaternion {
    type_t w, x, y, z;

    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(type_t _w, type_t _x, type_t _y, type_t _z)
        : w(_w), x(_x), y(_y), z(_z) {}
    Quaternion(type_t _w, const Vec3<type_t>& v)
        : w(_w), x(v.x), y(v.y), z(v.z) {}

    // ── Fábrica: rotação de 'angle' radianos em torno de 'axis' ──
    // Necessário para girar o carro
    static Quaternion fromAngleAxis(type_t angle, Vec3<type_t> axis) {
        axis = axis.normalized();
        type_t half = angle / type_t(2);
        type_t s    = std::sin(half);
        return Quaternion(std::cos(half), axis.x * s, axis.y * s, axis.z * s);
    }

    // ── Comprimento ──
    type_t length() const {
        return std::sqrt(w*w + x*x + y*y + z*z);
    }

    // ── Conjugado: q* = (w, -x, -y, -z) ──
    Quaternion conjugate() const {
        return Quaternion(w, -x, -y, -z);
    }

    // ── Inverso: q^-1 = conjugate / |q|^2 ──
    Quaternion inverse() const {
        type_t n = w*w + x*x + y*y + z*z;
        return Quaternion(w/n, -x/n, -y/n, -z/n);
    }

    // ── Normalização ──
    Quaternion normalized() const {
        type_t len = length();
        if (len == type_t(0)) return Quaternion(1, 0, 0, 0); // identidade
        return Quaternion(w/len, x/len, y/len, z/len);
    }

    // ── Multiplicação de quaternions: composição de rotações ──
    // q1 * q2 = aplicar q2 primeiro, depois q1
    Quaternion operator*(const Quaternion& o) const {
        return Quaternion(
            w*o.w - x*o.x - y*o.y - z*o.z,
            w*o.x + x*o.w + y*o.z - z*o.y,
            w*o.y - x*o.z + y*o.w + z*o.x,
            w*o.z + x*o.y - y*o.x + z*o.w
        );
    }

    Quaternion operator+(const Quaternion& o) const {
        return Quaternion(w+o.w, x+o.x, y+o.y, z+o.z);
    }
    Quaternion operator-(const Quaternion& o) const {
        return Quaternion(w-o.w, x-o.x, y-o.y, z-o.z);
    }
    Quaternion operator*(type_t f) const {
        return Quaternion(w*f, x*f, y*f, z*f);
    }

    // ── Rotaciona um vetor por este quaternion ──
    // Fórmula: v' = q * (0,v) * q^-1
    Vec3<type_t> rotate(const Vec3<type_t>& v) const {
        // Método eficiente sem construir quaternion intermediário
        Vec3<type_t> u(x, y, z);
        type_t s = w;
        Vec3<type_t> result =
            u * (u.dot(v) * type_t(2))
            + Vec3<type_t>(
                v.x * (s*s - u.dot(u)),
                v.y * (s*s - u.dot(u)),
                v.z * (s*s - u.dot(u))
              )
            + u.cross(v) * (type_t(2) * s);
        // Simplificado (fórmula de Rodrigues via quaternion):
        Vec3<type_t> t = u.cross(v) * type_t(2);
        return Vec3<type_t>(
            v.x + s * t.x + u.cross(t).x,
            v.y + s * t.y + u.cross(t).y,
            v.z + s * t.z + u.cross(t).z
        );
    }

    // ── Converte para Mat4 (para passar ao shader) ──
    // Necessário: UBO precisa de Mat4, não de Quaternion
    Mat4<type_t> toMat4() const {
        Quaternion q = normalized();
        type_t xx = q.x*q.x, yy = q.y*q.y, zz = q.z*q.z;
        type_t xy = q.x*q.y, xz = q.x*q.z, yz = q.y*q.z;
        type_t wx = q.w*q.x, wy = q.w*q.y, wz = q.w*q.z;

        Mat4<type_t> m;
        // Coluna 0
        m[0].x = 1 - 2*(yy+zz);
        m[0].y = 2*(xy+wz);
        m[0].z = 2*(xz-wy);
        // Coluna 1
        m[1].x = 2*(xy-wz);
        m[1].y = 1 - 2*(xx+zz);
        m[1].z = 2*(yz+wx);
        // Coluna 2
        m[2].x = 2*(xz+wy);
        m[2].y = 2*(yz-wx);
        m[2].z = 1 - 2*(xx+yy);
        // Coluna 3 já é (0,0,0,1) pelo Mat4 default
        m[3].w = 1;
        return m;
    }

    // ── Índice ──
    type_t& operator[](int i) {
        if (i < 0 || i >= 4) throw std::out_of_range("Índice inválido");
        return i == 0 ? w : (i == 1 ? x : (i == 2 ? y : z));
    }
    const type_t& operator[](int i) const {
        if (i < 0 || i >= 4) throw std::out_of_range("Índice inválido");
        return i == 0 ? w : (i == 1 ? x : (i == 2 ? y : z));
    }
};

template<typename type_t>
Quaternion<type_t> operator*(type_t f, const Quaternion<type_t>& q) {
    return q * f;
}

} // namespace MathVier

#endif
```

### Fix 2 — Vec3: adicionar operator*(scalar)

```cpp
// Em vector.hpp, dentro de struct Vec3<Type>, adicionar:
Vec3<Type> operator*(Type factor) const {
    return Vec3<Type>(x * factor, y * factor, z * factor);
}
Vec3<Type> operator/(Type factor) const {
    return Vec3<Type>(x / factor, y / factor, z / factor);
}
// Operador unário negação (útil para câmera)
Vec3<Type> operator-() const {
    return Vec3<Type>(-x, -y, -z);
}
// operator+= e -= (útil no loop de física)
Vec3<Type>& operator+=(const Vec3<Type>& o) {
    x += o.x; y += o.y; z += o.z; return *this;
}
Vec3<Type>& operator-=(const Vec3<Type>& o) {
    x -= o.x; y -= o.y; z -= o.z; return *this;
}
Vec3<Type>& operator*=(Type f) {
    x *= f; y *= f; z *= f; return *this;
}
```

### Fix 3 — Mat4: acesso aos dados para Vulkan

```cpp
// Em matrix.hpp, dentro de struct Mat4<Type>, adicionar:

// Necessário para memcpy ao UBO do Vulkan
const Type* data() const {
    return &columns[0].x;  // colunas são contíguas: alignas(16) + 4 floats cada
}
Type* data() {
    return &columns[0].x;
}
// Atalho: transpor (às vezes necessário dependendo da convenção row/col)
Mat4<Type> transposed() const {
    Mat4<Type> r;
    for (int c = 0; c < 4; ++c)
        for (int row = 0; row < 4; ++row)
            r.columns[row][c] = columns[c][row];
    return r;
}
```

### Fix 4 — Atualizar MathVier.hpp

```cpp
// include/MathVier/MathVier.hpp
#ifndef MATHVIER_HPP
#define MATHVIER_HPP

#include "array.hpp"
#include "vector.hpp"
#include "matrix.hpp"
#include "VectorGeneric.hpp"
#include "Quaternion.hpp"  // ← adicionar

#endif
```

### Testes da Pré-Fase (antes de qualquer engine)

```
[ ] Quaternion compila sem erros
[ ] Quaternion::fromAngleAxis(π/2, {0,1,0}).rotate({1,0,0}) ≈ {0,0,-1}
[ ] Quaternion::toMat4() aplicado em vec4(1,0,0,1) dá resultado correto
[ ] Vec3 operator* compila: Vec3<float>(1,0,0) * 5.0f == Vec3<float>(5,0,0)
[ ] Mat4::data() retorna ponteiro para columns[0].x
[ ] Mat4::perspective() * Mat4::lookAt() compila e retorna Mat4
```

---

## 🏁 FASE 0 — Re-entry (1–2 dias)

> Sem código novo no engine. Só relembrar e rodar.

```
[ ] tools/build_and_run.sh → triângulo na tela
[ ] Compilar shaders:
      glslc assets/shaders/core/basic.vert -o assets/shaders/core/compiled/vert.spv
      glslc assets/shaders/core/basic.frag -o assets/shaders/core/compiled/frag.spv
[ ] Rastrear: main() → run() → initVulkan() → drawFrame() → recordCommandBuffer()
[ ] Identificar o gap: createTriangle() cria buffer, recordCommandBuffer() não usa
[ ] Abrir RenderDoc, capturar frame, ver draw calls
[ ] Confirmar: validation layers sem erros
```

---

## 🔧 FASE 1 — Conectar o Vertex Buffer (2–3 dias)

> Primeira vez que C++ controla geometria. Buffer real na GPU.

### Dependência MathVier: nenhuma nova (usa floats brutos para posição/cor)

### O que implementar

**Vertex com binding/attribute descriptions** (em VulkanManager.hpp):

```cpp
struct Vertex {
    float pos[3];
    float color[3];

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription b{};
        b.binding   = 0;
        b.stride    = sizeof(Vertex);
        b.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return b;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> a{};
        a[0] = {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)};
        a[1] = {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)};
        return a;
    }
};
```

**recordCommandBuffer()** — adicionar antes do draw:

```cpp
VkBuffer vb[] = { bufferManager->getVkBuffer(vertexBuffer) };
VkDeviceSize offsets[] = { 0 };
vkCmdBindVertexBuffers(commandBuffer, 0, 1, vb, offsets);
vkCmdDraw(commandBuffer, 3, 1, 0, 0);
```

**Shader atualizado** (basic.vert):

```glsl
#version 450
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(inPos, 1.0);
    fragColor   = inColor;
}
```

### Testes da Fase 1

```
[ ] Triângulo aparece com cores vermelho/verde/azul
[ ] Mudar posições em createTriangle() → triângulo muda na tela
[ ] Mudar cores → cores mudam
[ ] Validation layers: zero erros, zero warnings
[ ] Resize continua funcionando
[ ] RenderDoc: vertex buffer visível nos resources do draw call
```

---

## 📦 FASE 2 — Index Buffer + Mesh System (3–4 dias)

> Geometria indexada. Cubo na tela.

### Dependência MathVier: nenhuma nova

### Mesh struct

```cpp
// include/rendering/Mesh.hpp
#pragma once
#include <core/BufferManager.hpp>
#include <vector>

struct MeshData {
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
};

class Mesh {
public:
    BufferHandle vertexBuffer = INVALID_HANDLE;
    BufferHandle indexBuffer  = INVALID_HANDLE;
    uint32_t     indexCount   = 0;

    static MeshData makeTriangle();
    static MeshData makeQuad();
    static MeshData makeCube();

    void upload(BufferManager& bm, const MeshData& data);
    void bind(VkCommandBuffer cmd, BufferManager& bm) const;
    void draw(VkCommandBuffer cmd) const;
};
```

**bind + draw**:

```cpp
void Mesh::bind(VkCommandBuffer cmd, BufferManager& bm) const {
    VkBuffer vb[] = { bm.getVkBuffer(vertexBuffer) };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, vb, offsets);
    vkCmdBindIndexBuffer(cmd, bm.getVkBuffer(indexBuffer), 0, VK_INDEX_TYPE_UINT32);
}
void Mesh::draw(VkCommandBuffer cmd) const {
    vkCmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);
}
```

### Testes da Fase 2

```
[ ] Quad (4v, 6i) renderiza sem artefatos
[ ] Cubo (8v, 36i) renderiza
[ ] Index count errado → validation layer avisa
[ ] 100 ciclos create/destroy sem leaks
```

---

## 🚗 FASE 3 — Carregar Modelo 3D do Carro (4–5 dias)

> Assimp já está linkado. Substitua o cubo pelo carro.

### Dependência MathVier: nenhuma nova (Assimp dá float[])

### ModelLoader básico

```cpp
// include/rendering/ModelLoader.hpp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelLoader {
public:
    ModelLoader(BufferManager& bm);
    std::vector<Mesh> load(const std::string& path);
private:
    BufferManager& bm;
    Mesh processMesh(const aiMesh* m);
};
```

### Onde pegar modelos de carro (grátis)

```
→ kenney.nl/assets         — low-poly, grátis, .gltf/.obj, perfeito
→ sketchfab.com/features/free — filtrar: free + low-poly + .glb
→ poly.pizza               — low-poly CC0
Poly count ideal para começar: 500–3000 triângulos
```

### AssetManager (caminhos portáveis)

```cpp
class AssetManager {
public:
    static void   init(const std::filesystem::path& root);
    static std::string model(const std::string& name);   // root/assets/models/
    static std::string shader(const std::string& name);  // root/assets/shaders/core/compiled/
    static std::string texture(const std::string& name); // root/assets/textures/
private:
    static std::filesystem::path root;
};
```

### Testes da Fase 3

```
[ ] Modelo .obj ou .gltf carrega sem crash
[ ] Aparece na tela (pode estar errado de ângulo/escala — normal)
[ ] Caminho inválido → exception com mensagem clara
[ ] Memory: destruir modelo → sem leaks (RenderDoc mostra 0 VkBuffer "vivos")
```

---

## 📷 FASE 4 — UBOs, Transforms e Câmera (4–5 dias)

> **Aqui a MathVier começa a trabalhar de verdade.**
> Tudo que você implementou em Mat4 — lookAt, perspective — vai ao Vulkan agora.

### Dependência MathVier: Mat4::lookAt, Mat4::perspective, Mat4::data()

### Como passar Mat4 para o UBO do Vulkan

```cpp
// Mat4<float> é column-major, 4 × Vec4<float>, contíguo em memória
// Isso é exatamente o que GLSL espera para uma mat4

struct UniformBufferObject {
    // Atenção: Vulkan espera floats brutos, 64 bytes por mat4
    float model[16];
    float view[16];
    float proj[16];
};

// Para preencher o UBO:
void fillUBO(UniformBufferObject& ubo,
             const MathVier::Mat4<float>& model,
             const MathVier::Mat4<float>& view,
             const MathVier::Mat4<float>& proj) {
    memcpy(ubo.model, model.data(), 64);
    memcpy(ubo.view,  view.data(),  64);
    memcpy(ubo.proj,  proj.data(),  64);
}
```

### Camera usando MathVier

```cpp
// include/rendering/Camera.hpp
#include <MathVier/MathVier.hpp>

class Camera {
public:
    MathVier::Vec3<float> position = {0, 3, -8};
    MathVier::Vec3<float> target   = {0, 0,  0};

    MathVier::Mat4<float> getViewMatrix() const {
        return MathVier::Mat4<float>::lookAt(
            position, target,
            MathVier::Vec3<float>(0, 1, 0)
        );
    }

    MathVier::Mat4<float> getProjectionMatrix(float aspect) const {
        auto proj = MathVier::Mat4<float>::perspective(
            0.7854f,  // 45° em radianos
            aspect,
            0.1f,
            1000.0f
        );
        // Vulkan tem Y invertido em relação ao OpenGL
        // Sua Mat4 usa column-major: columns[1].y é o elemento (1,1)
        proj[1].y *= -1.0f;
        return proj;
    }
};
```

### Shader com UBO

```glsl
// basic.vert
layout(set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPos, 1.0);
    fragColor   = inColor;
}
```

### DescriptorManager (esqueleto)

```cpp
class DescriptorManager {
public:
    DescriptorManager(VkDevice dev, uint32_t framesInFlight);
    ~DescriptorManager();

    VkDescriptorSetLayout getLayout()           const;
    VkDescriptorSet       getSet(uint32_t frame) const;

    // Atualiza o UBO do frame atual com novas matrizes
    void updateUBO(uint32_t frame, const UniformBufferObject& ubo,
                   BufferManager& bm);
private:
    VkDevice              device;
    VkDescriptorPool      pool;
    VkDescriptorSetLayout layout;
    std::vector<VkDescriptorSet> sets;
    std::vector<BufferHandle>    uboBuffers; // um por frame em flight
};
```

### Referências de estudo (Fase 4)

| Conceito | Onde estudar |
|---|---|
| MVP Matrices | [learnopengl.com/Getting-started/Transformations](https://learnopengl.com/Getting-started/Transformations) |
| Uniform Buffers + Descriptors | [vulkan-tutorial.com/Uniform_buffers](https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer) |
| Column-major vs row-major | [stackoverflow.com/questions/17717600](https://stackoverflow.com/questions/17717600) — entender por que Vulkan/GLSL espera column-major |
| lookAt matemática | [3D Math Primer, cap. 10](https://gamemath.com/) |

### Testes da Fase 4

```
[ ] Carro aparece no centro da tela (model = identity)
[ ] Mudar position da câmera → perspectiva muda
[ ] Carro rotaciona quando você muda model matrix com sin(time)
[ ] Resize: aspect ratio da projeção atualiza
[ ] Mat4::data() retorna os mesmos 16 floats que você colocou
[ ] Validation layers: descriptor sets sem erros
[ ] UBO atualiza por frame sem flickering
```

---

## ⌨️ FASE 5 — Input System (2–3 dias)

> Abstração limpa de teclado/mouse. Sem dependência de MathVier.

```cpp
// include/core/InputManager.hpp
class InputManager {
public:
    void initialize(GLFWwindow* w);
    void update();

    bool isKeyDown(int key)          const;
    bool isKeyJustPressed(int key)   const;
    bool isKeyJustReleased(int key)  const;
    MathVier::Vec2<float> getMouseDelta() const;
    void setCursorLocked(bool locked);
private:
    GLFWwindow* window;
    std::unordered_set<int> current, previous;
    MathVier::Vec2<float> mousePos, lastMousePos;
    bool firstMouse = true;
};
```

### Testes da Fase 5

```
[ ] W pressionado → isKeyDown(GLFW_KEY_W) == true
[ ] W liberado → false no próximo frame
[ ] isKeyJustPressed → true apenas no primeiro frame
[ ] Mouse delta: não explode no primeiro frame
[ ] ESC → cursor liberado
```

---

## 🚗 FASE 6 — Física Básica do Carro (5–7 dias)

> **Aqui o Quaternion finalmente entra em ação.**
> Rotação do carro sem gimbal lock, câmera que segue.

### Dependência MathVier: Quaternion::fromAngleAxis, Quaternion::rotate, Quaternion::toMat4

### Por que Quaternion em vez de ângulos Euler?

```
Ângulos Euler (yaw/pitch/roll):
  → Simples, mas sofrem de gimbal lock
  → O carro pode "travar" em certas orientações
  → Interpolação não funciona bem

Quaternion:
  → Representa rotação como (eixo, ângulo)
  → Sem gimbal lock
  → Multiplicação acumula rotações corretamente
  → toMat4() para passar ao shader
```

**Referência para entender:** [3D Math Primer, cap. 8](https://gamemath.com/) — Quaternions.
Também: [Visualização interativa](https://eater.net/quaternions) — Ben Eater.

### Classe Car usando MathVier

```cpp
// include/game/Car.hpp
#include <MathVier/MathVier.hpp>

class Car {
public:
    MathVier::Vec3<float>       position = {0, 0, 0};
    MathVier::Quaternion<float> rotation;  // identidade por padrão
    float speed   = 0.0f;
    float maxSpeed    = 20.0f;
    float acceleration = 15.0f;
    float brakeForce  = 20.0f;
    float turnSpeed   = 2.5f;
    float friction    = 0.92f;

    void update(float dt, bool accel, bool brake, float steer);

    MathVier::Vec3<float> getForward() const {
        // Z+ é a direção "frente" do carro
        return rotation.rotate(MathVier::Vec3<float>(0, 0, 1));
    }

    MathVier::Mat4<float> getModelMatrix() const {
        // T * R: primeiro rotaciona, depois translada
        auto T = MathVier::Mat4<float>::identity().translate(
            position.x, position.y, position.z);
        auto R = rotation.toMat4();
        return T * R;
    }
};
```

```cpp
// src/game/Car.cpp
void Car::update(float dt, bool accel, bool brake, float steer) {
    // Steering: só quando está se movendo (e proporcional à velocidade)
    if (std::abs(speed) > 0.1f) {
        float steerAmount = steer * turnSpeed * dt * (speed / maxSpeed);
        auto deltaRot = MathVier::Quaternion<float>::fromAngleAxis(
            steerAmount,
            MathVier::Vec3<float>(0, 1, 0)  // Eixo Y = virar
        );
        rotation = (deltaRot * rotation).normalized();
    }

    // Aceleração / freio
    if (accel) speed += acceleration * dt;
    if (brake) speed -= brakeForce * dt;

    // Atrito (exponencial, independente de framerate)
    speed *= std::pow(friction, dt * 60.0f);

    // Clamp de velocidade
    speed = std::max(-maxSpeed * 0.5f, std::min(speed, maxSpeed));

    // Mover na direção que o carro aponta
    position += getForward() * (speed * dt);
}
```

### Câmera terceira pessoa

```cpp
void updateCamera(Camera& cam, const Car& car) {
    // Offset: 8 unidades atrás, 3 acima
    auto offset = car.rotation.rotate(MathVier::Vec3<float>(0, 3, -8));
    cam.position = car.position + offset;
    cam.target   = car.position + MathVier::Vec3<float>(0, 1, 0);
}
```

### Game loop completo

```cpp
float lastTime = (float)glfwGetTime();
while (!window.shouldClose()) {
    float now = (float)glfwGetTime();
    float dt  = now - lastTime;
    lastTime  = now;

    input->update();

    bool accel = input->isKeyDown(GLFW_KEY_W);
    bool brake = input->isKeyDown(GLFW_KEY_S);
    float steer = input->isKeyDown(GLFW_KEY_D) ? 1.0f
                : input->isKeyDown(GLFW_KEY_A) ? -1.0f : 0.0f;

    car->update(dt, accel, brake, steer);
    updateCamera(*camera, *car);

    // Preencher UBO com MathVier
    UniformBufferObject ubo{};
    auto model = car->getModelMatrix();
    auto view  = camera->getViewMatrix();
    auto proj  = camera->getProjectionMatrix(aspect);
    memcpy(ubo.model, model.data(), 64);
    memcpy(ubo.view,  view.data(),  64);
    memcpy(ubo.proj,  proj.data(),  64);
    descriptorManager->updateUBO(currentFrame, ubo, *bufferManager);

    window.pollEvents();
    drawFrame();
}
```

### Testes da Fase 6

```
[ ] W → carro acelera para frente
[ ] S → freia/recua
[ ] A/D → vira (mais pronunciado em alta velocidade)
[ ] Soltar tudo → para gradualmente
[ ] Câmera segue sem "saltar"
[ ] 60 FPS estável durante física
[ ] Frame-rate independent: mesma sensação em 30fps e 144fps
[ ] Quaternion::fromAngleAxis seguido de rotate dá resultado correto
[ ] getModelMatrix() produz matrix correta (verificar no RenderDoc)
```

---

## 🏟️ FASE 7 — Pista e Depth Buffer (3–4 dias)

> Depth buffer para 3D real. Pista abaixo do carro.

### Dependência MathVier: nenhuma nova

### Depth buffer (crítico — sem isso objetos se sobrepõem errado)

```cpp
// Em PipelineManager, habilitar depth:
VkPipelineDepthStencilStateCreateInfo depth{};
depth.sType            = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
depth.depthTestEnable  = VK_TRUE;
depth.depthWriteEnable = VK_TRUE;
depth.depthCompareOp   = VK_COMPARE_OP_LESS;

// Em SwapchainManager: adicionar depth attachment ao render pass
// (ver: vulkan-tutorial.com/Depth-buffering para código completo)
```

### Testes da Fase 7

```
[ ] Pista aparece abaixo do carro
[ ] Carro não atravessa a pista
[ ] Objetos mais próximos aparecem na frente dos distantes
[ ] Resize: depth buffer recriado corretamente
```

---

## 💡 FASE 8 — Iluminação Phong (5–6 dias)

> **MathVier normaliza vetores no C++. O shader faz o cálculo de luz.**

### Dependência MathVier: Vec3::normalized, Vec3::dot (já existem)

### Vertex com normal

```cpp
struct Vertex {
    float pos[3];
    float color[3];
    float normal[3];  // NOVO — 3 attributes agora

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        return {{
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)},
            {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)},
        }};
    }
};
```

### UBO expandido

```cpp
struct UniformBufferObject {
    float model[16];
    float view[16];
    float proj[16];
    float lightPos[4];   // xyz = posição, w = intensidade
    float lightColor[4]; // rgb = cor
    float viewPos[4];    // xyz = câmera (para specular)
};

// Preencher viewPos com MathVier:
auto vp = camera->position;
ubo.viewPos[0] = vp.x; ubo.viewPos[1] = vp.y; ubo.viewPos[2] = vp.z; ubo.viewPos[3] = 1;
```

### Fragment shader Phong

```glsl
layout(location = 0) in vec3 fragPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragColor;

layout(set = 0, binding = 0) uniform UBO {
    mat4 model; mat4 view; mat4 proj;
    vec4 lightPos; vec4 lightColor; vec4 viewPos;
} ubo;

void main() {
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(ubo.lightPos.xyz - fragPos);
    vec3 V = normalize(ubo.viewPos.xyz  - fragPos);
    vec3 H = normalize(L + V);

    float ambient  = 0.15;
    float diffuse  = max(dot(N, L), 0.0);
    float specular = pow(max(dot(N, H), 0.0), 32.0) * 0.5;

    vec3 result = (ambient + diffuse + specular) * fragColor * ubo.lightColor.rgb;
    outColor = vec4(result, 1.0);
}
```

### Referências (Fase 8)

| Conceito | Onde estudar |
|---|---|
| Phong shading | [learnopengl.com/Lighting/Basic-Lighting](https://learnopengl.com/Lighting/Basic-Lighting) |
| Normal matrix | Quando escalar não-uniformemente, a normal precisa de transpose(inverse(model)). Veja [learnopengl.com/Lighting/Basic-Lighting](https://learnopengl.com/Lighting/Basic-Lighting) seção Normal Matrix |
| Blinn-Phong | [learnopengl.com/Advanced-Lighting/Advanced-Lighting](https://learnopengl.com/Advanced-Lighting/Advanced-Lighting) |

### Testes da Fase 8

```
[ ] Carro tem gradiente de luz vs sombra
[ ] Mover luz → iluminação muda em tempo real
[ ] Specular visível em ângulos corretos
[ ] Normais carregadas corretamente do modelo Assimp
[ ] 60 FPS com iluminação
```

---

## 🎨 FASE 9 — Texturas (4–5 dias)

> stb_image para carregar. Sampler Vulkan para filtrar.

### Dependência MathVier: nenhuma

### Vec2 entra aqui: UV coordinates

```cpp
struct Vertex {
    float pos[3];
    float normal[3];
    float uv[2];      // Coordenadas de textura (substitui color)
};
```

### TextureManager

```cpp
class TextureManager {
public:
    struct Texture {
        VkImage       image;
        VkImageView   view;
        VkSampler     sampler;
        VmaAllocation allocation;
        uint32_t      width, height;
    };

    Texture load(const std::string& path);
    void    destroy(Texture& t);
private:
    // Helpers internos:
    // createImage, transitionLayout, copyBufferToImage, generateMipmaps
};
```

### Referências (Fase 9)

```
vulkan-tutorial.com/Texture_mapping/Images       — criar VkImage
vulkan-tutorial.com/Texture_mapping/Image_view   — VkImageView
vulkan-tutorial.com/Texture_mapping/Sampler      — filtros, mipmaps
github.com/nothings/stb (stb_image.h)            — loading de PNG/JPG
```

### Testes da Fase 9

```
[ ] Textura do carro aparece corretamente
[ ] Textura de asfalto na pista
[ ] Caminho inválido → exception clara
[ ] Mipmaps: qualidade muda com distância
[ ] Memory: destroy libera VkImage e VmaAllocation
```

---

## 🖥️ FASE 10 — HUD com ImGui (2–3 dias)

> ImGui já está no projeto. Velocímetro, debug de física.

```cpp
ImGui::Begin("Speed Racer");
ImGui::Text("Speed:    %.1f km/h",  car->speed * 3.6f);
ImGui::Text("Position: %.1f, %.1f", car->position.x, car->position.z);
ImGui::Text("FPS:      %.0f",       1.0f / deltaTime);
ImGui::Separator();
ImGui::Text("--- Tuning ---");
ImGui::SliderFloat("Max Speed",    &car->maxSpeed,     5,  50);
ImGui::SliderFloat("Acceleration", &car->acceleration, 1,  30);
ImGui::SliderFloat("Turn Speed",   &car->turnSpeed,   0.5f, 5);
ImGui::SliderFloat("Friction",     &car->friction,   0.80f, 0.99f);
ImGui::End();
```

---

## 🔮 FASE 11+ — Roadmap da MathVier (paralelo, sem pressa)

À medida que o engine evolui, você vai precisar expandir a MathVier:

```
[ ] Mat4::inverse()              — necessário para normal matrix na iluminação
[ ] Vec3::lerp(v, t)             — interpolação suave de câmera
[ ] Quaternion::slerp(q, t)      — rotação suave entre orientações
[ ] Mat4::orthographic()         — para shadow maps e UI
[ ] Vec3 operadores +=, -=, *=   — conveniência no loop de física
[ ] Testes unitários com GTest   — cobertura de toda a lib
```

**Futuro mais distante:**

```
[ ] SIMD (SSE/AVX) para Vec4/Mat4 — otimização
[ ] ECS (Entity Component System) — múltiplos carros
[ ] PBR shading                   — iluminação fisicamente correta
[ ] Shadow mapping                — sombras (Sascha Willems examples)
[ ] Skybox                        — cubo de ambiente
```

---

## 📋 Setup: MathVier no CMakeLists do Speed Racer

```cmake
# Adicionar a lib ao projeto:
add_subdirectory(libs/MathVier)  # ou onde ela estiver

# Ou se for header-only incluir direto:
target_include_directories(Speed_Racer PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/MathVier/include
)

# Nenhum link necessário se for header-only com .cpp separado:
# Se tiver vector.cpp e matrix.cpp, compile-os:
target_sources(Speed_Racer PRIVATE
    libs/MathVier/src/vector.cpp
    libs/MathVier/src/matrix.cpp
)
```

### Estrutura de pastas recomendada para a MathVier

```
libs/MathVier/
├── include/
│   └── MathVier/
│       ├── MathVier.hpp        ← include único
│       ├── vector.hpp
│       ├── matrix.hpp
│       ├── Quaternion.hpp      ← corrigido
│       ├── VectorGeneric.hpp
│       └── array.hpp
├── src/
│   ├── vector.cpp
│   └── matrix.cpp
└── tests/
    ├── test_vector.cpp
    ├── test_matrix.cpp
    └── test_quaternion.cpp
```

---

## 📅 Estimativa Revisada

| Fase | Dias | Entrega |
|---|---|---|
| **Pré-Fase MathVier** | 1–2 | Quaternion funciona, Vec3 completo |
| 0 — Re-entry | 1–2 | Código relembrado |
| 1 — Vertex Buffer | 2–3 | Triângulo controlado por C++ |
| 2 — Mesh System | 3–4 | Cubo indexado |
| 3 — Modelo do carro | 4–5 | Carro 3D estático |
| 4 — UBO + Câmera | 4–5 | MathVier::Mat4 no shader Vulkan ← **marco** |
| 5 — Input | 2–3 | WASD funcionando |
| 6 — Física do carro | 5–7 | **CARRO DIRIGINDO** 🎉 ← **marco maior** |
| 7 — Pista + Depth | 3–4 | Carro em pista 3D |
| 8 — Iluminação | 5–6 | Parece um jogo |
| 9 — Texturas | 4–5 | Parece um jogo bonito |
| 10 — HUD | 2–3 | Polido |
| **Total** | **~6–9 semanas** | Protótipo jogável |

---

## 💬 Por que usar sua própria lib matemática importa

Qualquer projeto usa GLM ou DirectXMath. **Você está fazendo diferente.**

Quando o carro girar na tela e a câmera seguir, o quaternion que fez aquilo é código que você escreveu. A matriz que foi para o shader GPU foi montada pelo seu `Mat4::lookAt`. Isso é raro — e é exatamente o tipo de coisa que diferencia um portfólio técnico de um tutorial copiado.

**A regra:** Use MathVier onde ela já funciona. Adicione à lib o que precisar conforme o engine pede. Não pare o engine pra fazer a lib perfeita — elas crescem juntas.

Bora. 🏎️💨
