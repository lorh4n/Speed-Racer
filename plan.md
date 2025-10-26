# 🏎️ Speed Racer Engine – Roadmap v2.1

## Contexto Atual
- `VulkanManager` concentra orquestração, criação de device, render pass e ciclo de draw (ver `include/core/VulkanManager.hpp`), reforçando a necessidade de DI e composição gradual.
- Managers existentes (`SwapchainManager`, `CommandManager`, `RenderPassManager`) já atuam como Facade; evolução deve manter essa filosofia e eliminar singletons mencionados nas versões anteriores do plano.
- Pipelines usam caminhos relativos rígidos (`src/core/PipelineManager.cpp`) e não há build automatizado de shaders, confirmando prioridade para toolchain no CMake.
- Sync de frames e recriação de swapchain funcionam, mas ainda falta validação sistemática (RenderDoc, MAX_FRAMES_IN_FLIGHT variável) prevista nas métricas semanais.

## Padrões por Responsabilidade
| Responsabilidade | Padrão | Observações |
| --- | --- | --- |
| Composition root (`VulkanManager`) | Composition Root + Mediator leve | Instancia e injeta managers explícitos, evitando god-object; responsável por orquestrar lifecycle e liberar recursos. |
| `ResourceManager` | RAII Handles + Flyweight | Mapas de handles para buffers/images; compartilhamento e reciclagem de recursos, expondo ponteiros Vulkan sob demanda. |
| `BufferManager` | Facade + Strategy | Estratégias para buffers device-local vs host-visible; usa `CommandManager` via DI para cópias one-shot. |
| `PipelineBuilder` | Builder + Adapter | Builder fluente; Adapter converte manifest JSON em `ShaderReflection`. `PipelinePresets` atua como Director. |
| `DescriptorManager` | Abstract Factory + Object Pool | Produz pools, layouts e sets com reuso controlado; integra ResourceManager para handles de buffer/image. |
| `Scene`/`SceneNode` | Composite + Iterator | Hierarquia de nós com traversal em profundidade; prepara terreno para ECS. |
| `FrameGraph` | DAG Scheduler + Command pattern | Passes registrados com dependências, ordenação topológica e invocação encapsulada. |
| `TextureManager`/`MaterialManager` | Facade + Flyweight | Cache de texturas e materiais; push constants e descriptor sets compartilhados. |
| Sistemas de luz e câmera | Observer opcional + Strategy | Atualizações propagadas para UBOs; diferentes câmeras/luzes implementadas como estratégias. |

## Sequência Geral
- Semanas 1-4 consolidam fundamentos (memória, buffers, cena, descriptors, câmera, profundidade, modelos).
- Semanas 5-6 ampliam materiais e iluminação, já com infraestrutura pronta para performance e hot-reload.
- Semanas 7-8 introduzem camada de interação/ECS mantendo scene graph como base.
- Cada semana inclui janelas formais de validação (CI, RenderDoc, métricas) para evitar regressões acumuladas.

## Semana 1 – Fundação Arquitetural
**Objetivo**: Migrar infraestrutura para DI limpa, introduzir VMA e automatizar toolchain de shaders sem quebrar o triângulo atual.

**Padrões-chave**
- Composition Root no `VulkanManager` com injeção explícita.
- Facade/Strategy em `BufferManager`, RAII/Flyweight em `ResourceManager`.
- Builder + Adapter no `PipelineBuilder`; Facade leve para `AssetManager`.

**Checklist**
- [ ] Integrar Vulkan Memory Allocator (`VmaAllocator` criado em `VulkanManager::initVulkan`) e encapsular em `VmaBuffer`/`VmaImage`.
- [ ] Implementar `ResourceManager` com handles RAII, `HandleAllocator` e testes criando/destruindo buffers.
- [ ] Criar `BufferManager` Facade com strategies para staging/device e upload via `CommandManager`.
- [ ] Adicionar target `compile_shaders` no CMake, gerar manifest JSON e `PipelineBuilder` consumindo reflection.
- [ ] Normalizar `AssetManager` com diretórios configuráveis (config JSON) e logging centralizado de assets.
- [ ] Validar 2→3 frames in flight, swapchain recreation repetida, debug markers + timestamp básicos.

**Métricas de sucesso**
- Triângulo renderizado após refatoração sem warnings nas validation layers.
- Criação/destruição de 100 buffers sem vazamentos ou double free.
- Build recompila shaders automaticamente ao alterar `.vert/.frag`.
- CI local executa `cmake --build` + teste simples em <15 min.
- RenderDoc captura mostrando markers por estágio.

**Exemplo rápido**
```cpp
class BufferManager {
public:
    BufferHandle create(const BufferDesc& desc);     // Escolhe strategy conforme uso
    void upload(BufferHandle h, gsl::span<const std::byte> data);
private:
    IBufferStrategy& pickStrategy(BufferUsage usage);
};
```

## Semana 2 – Geometria e Cena
**Objetivo**: Expandir do triângulo para múltiplos objetos com cena hierárquica e mini framegraph, mantendo perfis e validação.

**Padrões-chave**
- Vertex/Index buffer usando Facade (`BufferManager`) e RAII.
- Composite (`SceneNode`) com Iterator para traversal.
- FrameGraph como DAG Scheduler com Command encapsulado.
- Mesh/geometry helpers como Flyweight simples (cache de dados).

**Checklist**
- [ ] Definir `Vertex` (pos/normal/uv/color) e index buffer usando Resource/Buffer managers.
- [ ] Atualizar pipeline/shaders para `vkCmdBindVertexBuffers` + `vkCmdDrawIndexed`.
- [ ] Implementar `Scene`, `SceneNode`, `Transform` com traversal DFS e integração no render loop.
- [ ] Introduzir `FrameGraph` com passes “Forward” e “UI”, topological sort simples e debug labels.
- [ ] Criar `Mesh` helpers (`createCube`, `createPlane`) e adicionar 10 objetos variados com transforms.
- [ ] Profiling inicial: timestamp queries, relatório de FPS/memória, gatilho RenderDoc.

**Métricas de sucesso**
- Cena com 10+ objetos renderizada a 60 FPS @1080p.
- Travessia do scene graph sem alocações durante frame.
- FrameGraph mostra passes e dependências corretas nos markers.
- Upload de geometria amortizado (batch staging) sem leaks.

**Exemplo rápido**
```cpp
class SceneNode {           // Composite
public:
    void addChild(SceneNode& child);
    glm::mat4 worldMatrix() const;
private:
    SceneNode* parent{};
    std::vector<SceneNode*> children;
};
```

## Semana 3 – Descriptors, Texturas e Câmera
**Objetivo**: Habilitar UBOs, texturas e câmera FPS estável usando gerenciadores especializados.

**Padrões-chave**
- `DescriptorManager` como Abstract Factory + pool recycling.
- `TextureManager` Facade com cache (Flyweight) e integração com `ResourceManager`.
- `Camera` Strategy para diferentes comportamentos (FPS/orbit).
- Atualizações Observers para sincronizar câmera/luzes com UBOs.

**Checklist**
- [ ] Criar `DescriptorManager` (pools, layouts, allocate/update) e UBO por frame in flight.
- [ ] Atualizar pipeline/layout para set 0 (UBO) + set 1 (sampler), garantindo reflection combina.
- [ ] Implementar `TextureManager` com stb_image, staging + layout transitions e mipmaps opcionais.
- [ ] Estender `Vertex` com UV, atualizar shaders e descriptor writes para texturas.
- [ ] Construir `Camera` (view/projection, input WASD + mouse) com delta time suave.
- [ ] Registrar métricas de jitter, revisar RenderDoc/Tracy para garantir ausência de stalls.

**Métricas de sucesso**
- Cena texturizada com múltiplos objetos, sem flicker em troca de frames.
- Rotação e movimento da câmera fluem a 60 FPS sem jitter.
- Loader de texturas reutiliza imagens duplicadas (cache acerta >90% em cena teste).
- Descriptor reuse evita recriações durante frame.

**Exemplo rápido**
```cpp
struct DescriptorSetRef {
    DescriptorPoolHandle pool;
    VkDescriptorSet set;
}; // Produced by DescriptorManager factory
```

## Semana 4 – Profundidade e Modelos
**Objetivo**: Ativar depth testing, carregar modelos reais (Assimp) e solidificar pipeline cache/CI.

**Padrões-chave**
- Depth buffer tratado como recurso RAII pelo `ResourceManager`.
- `ModelLoader` como Adapter sobre Assimp, delegando buffers ao BufferManager.
- `MaterialManager` (Flyweight) preparando terreno para iluminação.
- Pipeline cache persistido como Strategy de inicialização.

**Checklist**
- [ ] Criar image depth (VMA) + image view, atualizar render pass/framebuffers para depth.
- [ ] Validar depth test (cenas com sobreposição) e garantir ausência de z-fighting.
- [ ] Implementar `ModelLoader` (OBJ/Assimp) gerando `Mesh` + materiais, cacheando buffers.
- [ ] Integrar `MaterialManager` básico (cores/texturas) e binding via descriptors/push constants.
- [ ] Persistir pipeline cache (`vkGetPipelineCacheData`) e recarregar no boot.
- [ ] Expandir CI: build Debug + smoke test carregando modelo/textura.

**Métricas de sucesso**
- Dois modelos complexos (>50k polígonos) renderizados a 60 FPS.
- Pipeline cache reduz build subsequente >30%.
- Log de carregamento evidencia reúso de buffers/texturas entre modelos.
- Valgrind/ASan sem vazamentos ao carregar/destruir modelos.

**Exemplo rápido**
```cpp
class ModelLoader { // Adapter sobre Assimp
public:
    std::unique_ptr<Model> load(const std::string& path);
private:
    Mesh buildMesh(const aiMesh& mesh, const aiScene& scene);
};
```

## Semanas 5-6 – Iluminação e Materiais
- Implementar `LightManager` Observer sincronizando cena → UBO de iluminação; shaders Blinn-Phong com suporte a múltiplas luzes.
- Expandir `MaterialManager` (Flyweight) com push constants, normal/specular maps e editor ImGui para hot-reload.
- Adicionar frustum culling básico (BoundingBox no scene graph) e preparar infraestrutura para occlusion.
- Metas: 100+ objetos com 10 luzes a 60 FPS, hot-reload de materiais em <1s, testes/CI cobrindo pipeline de materiais.

## Semanas 7-8 – Interatividade e ECS
- Introduzir ECS leve (ou Component System modular) sobre o scene graph, mantendo Composite como fallback.
- Abstrair input (teclado/mouse/gamepad) e game loop com delta time suave; sincronizar com `Camera` strategies.
- Preparar scripting/hot-reload de cenas, serialization e editor minimalista.
- Metas: 5 sistemas (render, input, movement, lights, UI) operando sem acoplamento, profiling confirmando frame budget.

## Janelas Fixas de Validação
- Final da Semana 1: `cmake --build`, testes de Resource/Buffer, captura RenderDoc, revisão de warnings.
- Final da Semana 2: Benchmark com 10+ objetos e relatório Tracy/Validation Layers.
- Final da Semana 3: Teste de movimentação câmera + carga de texturas múltiplas.
- Final da Semana 4: CI rodando smoke test com modelo complexo, revisão de memória com Valgrind/ASan.

## Notas de Avaliação
- Planejamento agora está alinhado ao estado atual do código e design patterns adequados para cada subsistema.
- Dependências críticas (ex.: VMA antes de BufferManager, manifest antes de PipelineBuilder) estão explícitas nos checklists.
- Código de exemplo foi reduzido a skeletons para facilitar entendimento sem poluir o README.
- Métricas e janelas de validação distribuídas por semana evitam acumular risco técnico ao final das fases.
