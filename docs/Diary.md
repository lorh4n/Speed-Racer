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

Dia de refatoração pesada e abstração de geometria! 🛠️💎

- **Abstração de Geometria (Classe `Mesh`):** Finalmente criei uma classe dedicada pra gerenciar os objetos 3D. O `Mesh` agora encapsula os handles de buffers (vértices e índices) e sabe como se preparar para o desenho (`bind`) e como enviar o comando de renderização (`draw`). Isso removeu toda aquela lógica manual "solta" que estava no `VulkanManager`.
- **Refatoração e Boas Práticas:**
    - Movi a implementação do `Mesh` para um arquivo `.cpp` dedicado. Isso ajuda a manter os cabeçalhos limpos e melhora o tempo de compilação.
    - Implementei **Move Semantics** (`&&` operators) para garantir que possamos transferir meshes sem copiar dados pesados da GPU por acidente.
    - Adicionei proteção contra **Memory Leak**: agora, se você subir novos dados para um Mesh que já existe, ele limpa automaticamente os buffers antigos na placa de vídeo.
- **`MeshFactory`:** Criei um gerador de geometrias básicas (Cubo, Triângulo e Quadrado). Agora, criar um cubo é tão simples quanto chamar `MeshFactory::makeCube()`.
- **Melhorias no `BufferManager`:**
    - Implementei a função `destroyBuffer` que recebe uma referência para o handle. Isso permite que o gerente invalide o handle na origem (setando como `INVALID_HANDLE`), evitando erros de "double destruction".
- **Integração no `VulkanManager`:**
    - O cubo agora é gerenciado por um `std::unique_ptr<Mesh>`, garantindo que ele seja destruído no momento certo e não cause erros de construtor padrão durante a inicialização do motor.
    - Limpamos a função `recordCommandBuffer`, que agora conversa com objetos `Mesh` em vez de manipular buffers brutos do Vulkan.

A arquitetura da engine deu um salto enorme hoje. O código está muito mais profissional, fácil de expandir e seguro contra vazamentos de memória. 🧊⚙️🚀
