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

# 12 de maio de 2025
 
 Descobri que eu sou obrigado a fazer um destrutor caso eu faça um construtor
 implementei o basico deo WindowManager
uma recomendaões de IA é não usar o glad pq eu to mexendo como vulkan ent usaria outra coisa
mas posso me arrepender se for adicionar o opengl na engine depois???