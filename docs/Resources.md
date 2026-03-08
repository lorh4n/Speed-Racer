# 📦 Fluxo de Recursos e Memória

O Speed Racer utiliza um sistema de transferência de dados otimizado para garantir que a geometria e as texturas fiquem na memória mais rápida da GPU (`DEVICE_LOCAL`).

---

## 🚀 Transferência de Dados (Staging)

Quando você chama `BufferManager::createVertexBuffer`, o motor realiza os seguintes passos:

1.  **Criação do Staging Buffer**: Um buffer temporário é criado com a flag `VMA_MEMORY_USAGE_CPU_TO_GPU`. Ele é visível para a CPU.
2.  **Mapeamento e Cópia**: Os dados brutos (vetor de vértices) são copiados para este buffer via `memcpy`.
3.  **Criação do Local Buffer**: O buffer definitivo é criado na GPU com a flag `VMA_MEMORY_USAGE_GPU_ONLY`. A CPU não consegue ler/escrever aqui diretamente.
4.  **Comando de Cópia**: O motor aloca um `VkCommandBuffer` temporário, grava um comando `vkCmdCopyBuffer` e submete à fila de transferência.
5.  **Sincronização**: O motor aguarda a GPU terminar a cópia (`vkQueueWaitIdle`) antes de liberar o Staging Buffer.

Este fluxo é necessário porque a memória `DEVICE_LOCAL` é muito mais rápida para a GPU processar durante o desenho, mas inacessível diretamente pelo processador.

---

## 🛡️ Gestão RAII e Segurança

### `ScopedBuffer`
Utilizado internamente para garantir que buffers temporários não vazem. Quando um `ScopedBuffer` sai de escopo, seu destrutor chama `vmaDestroyBuffer` automaticamente.

### Destruição de Handles
Ao chamar `bufferManager->destroyBuffer(handle)`, o `ResourceManager` libera a memória VMA e remove a entrada do mapa. O handle original é passado por referência e setado como `INVALID_HANDLE`, evitando que o programador tente usar um recurso que não existe mais.

---

## 📊 Tipos de Buffer Suportados

| Tipo | Uso Principal | Localização |
| :--- | :--- | :--- |
| **Vertex** | Armazenar `pos` e `color`. | GPU Only |
| **Index** | Definir a ordem de desenho. | GPU Only |
| **Uniform** | Matrizes de transformação (MVP). | CPU to GPU |
| **Staging** | Transferência intermediária. | CPU to GPU |
