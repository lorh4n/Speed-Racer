# 🏁 Diário de Desenvolvimento — Speed Racer

Esse diário é uma continuação do diário de quando eu estava estudando OpenGL. [LINK<>]

---

## 📅 10 de Maio de 2025

Decidi fazer o Speed Racer direto aqui no GitHub que já tinha criado. A engine vai ser em Vulkan, e é isso aí.

Simborraaa!! 🚗💨

> Primeira coisa que queria comentar: o uso de IA nesse projeto vai ser meramente organizacional, ou seja, vou pedir só recomendações sobre boas práticas de programação e arquitetura do projeto. Mas, sem querer ser hipócrita, posso acabar usando se eu estiver me ferrando muito e não conseguir encontrar um bug. Obs: uso IA pra formatar esse diário também, não vou gastar tempo formatando markdown.

Aprendi usar o cmake direito?? talvez... definitivamente não.

---

### 📁 Estrutura Básica do Projeto

```plaintext
Speed-Racer/
├── assets/                # Recursos do jogo (imagens, sons, etc)
├── build/                 # Arquivos de build (gerado pelo CMake)
├── docs/                  # Documentação do projeto
│   └── Diary.md           # Este diário
├── include/               # Headers (.hpp)
│   └── core/
│       └── VulkanManager.hpp
├── libs/                  # Bibliotecas externas
├── LICENSE                # Licença do projeto
├── src/                   # Código-fonte (.cpp)
│   ├── app/
│   │   └── main.cpp
│   └── core/              # Implementação dos hearders do include/core
└── tools/                 # Scripts e ferramentas auxiliares
    ├── build_and_run.sh
    └── run.sh
```

## 12 de maio de 2025
 
 Descobri que eu sou obrigado a fazer um destrutor caso eu faça um construtor
 implementei o basico deo WindowManager
uma recomendaões de IA é não usar o glad pq eu to mexendo como vulkan ent usaria outra coisa
mas posso me arrepender se for adicionar o opengl na engine depois???

## 13 de maio de 2025
implemntei o a parte do instance do tutoiral link (https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Instance)
vamos agora (10:47) pra validation layers, eu vou pedir ajuda pra ia nessa parte pq na minha cabeça é muito complexo isso
a partir daqui eu vou fazer o uml ou diagramas pra ficar melhor

---

## 📅 01 de Março de 2026

Hoje foi dia de "arrumar a casa" depois de formatar o PC e trazer features importantes de outras branches pra main.

- **Portabilidade e CMake:** Atualizei o `CMakeLists.txt` pra usar `add_subdirectory` com as bibliotecas na pasta `libs/` (`glfw`, `glm`, `VMA`). Isso é muito melhor porque o projeto fica mais "autossuficiente" e não dá erro de "arquivo não encontrado" toda vez que mudar de PC.
- **Validation Layers:** Como o PC estava limpo, o Vulkan dava crash logo de cara. Melhorei o `VulkanManager` pra ser mais robusto: agora ele verifica se as camadas de validação existem antes de tentar usar. Se não tiver, ele avisa e continua rodando em vez de explodir. Aproveitei e listei as dependências no `readme.md`.
- **Do Triângulo ao Cubo (Branch Devil):** Finalmente joguei o cubo 3D pra main!
    - **Push Constants:** Implementei pra mandar a matriz de transformação (MVP) direto pro shader sem precisar de Descriptor Sets por enquanto.
    - **Indexed Drawing:** Mudei a lógica de desenho pra usar `vkCmdDrawIndexed`. É muito mais limpo pra objetos complexos como o cubo.
    - **Matemática:** Usei a `glm` pra calcular a rotação em tempo real com base no clock do sistema.
- **Limpeza:** Movi a struct `Vertex` pra `ResourceTypes.hpp` pra deixar os cabeçalhos mais organizados e fáceis de manter.

O motor agora inicializa certinho, o build tá liso e o cubo tá lá girando! 🧊✨


Roadmap de implementção esta no Notion

---

## 📅 07 de Março de 2026

Dia de refatoração pesada e abstração de geometria. 🛠️💎

- **Classe `Mesh`:** Criei a classe pra encapsular os buffers e a lógica de `bind` e `draw`. Movi a implementação pro `.cpp`, adicionei Move Semantics e proteção contra memory leak nos setters.
- **`MeshFactory`:** Namespace para gerar geometrias básicas (Cubo, Triângulo, etc).
- **`BufferManager`:** Implementei `destroyBuffer` com referência pra limpar o handle automaticamente.
- **VulkanManager:** Mudei os testes pra usar `std::unique_ptr<Mesh>` e limpei o `recordCommandBuffer`.
- **Documentação:** Comecei a documentação técnica oficial na pasta `docs/`.

---

## 📅 08 de Março de 2026

Consegui carregar modelos reais usando Assimp. 🏎️💨

- **Assimp:** Integrei a lib e configurei o CMake pra compilar direto. Agora dá pra importar `.obj`, `.fbx`, etc.
- **ModelLoader:** Criei a classe pra converter os dados do Assimp pro meu `MeshData`.
- **VulkanManager:** Movi o vetor de meshes pra dentro da classe (corrigindo erro de definições múltiplas) e agora carrego o modelo da ambulância.

### To-do:
- [ ] Implementar texturas (o modelo está branco porque falta ler o `.mtl` e as imagens).
- [ ] Sistema de câmera rotativa.
- [ ] Ajustar escalas dos modelos.
