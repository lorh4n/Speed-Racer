#include <core/Mesh.hpp>

Mesh::Mesh(BufferManager *bufferMgr) : vertexBuffer(INVALID_HANDLE),
                                       indexBuffer(INVALID_HANDLE),
                                       indexCount(0),
                                       bufferManager(bufferMgr) {
}

Mesh::~Mesh() {
	cleanup();
}

Mesh::Mesh(Mesh &&other) noexcept
    : vertexBuffer(other.vertexBuffer),
      indexBuffer(other.indexBuffer),
      indexCount(other.indexCount),
      bufferManager(other.bufferManager) {
	other.vertexBuffer  = INVALID_HANDLE;
	other.indexBuffer   = INVALID_HANDLE;
	other.indexCount    = 0;
	other.bufferManager = nullptr;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
	if (this != &other) {
		cleanup();

		vertexBuffer  = other.vertexBuffer;
		indexBuffer   = other.indexBuffer;
		indexCount    = other.indexCount;
		bufferManager = other.bufferManager;

		other.vertexBuffer  = INVALID_HANDLE;
		other.indexBuffer   = INVALID_HANDLE;
		other.indexCount    = 0;
		other.bufferManager = nullptr;
	}
	return *this;
}

void Mesh::cleanup() {
	if (bufferManager) {
		if (vertexBuffer != INVALID_HANDLE) {
			bufferManager->destroyBuffer(vertexBuffer);
		}
		if (indexBuffer != INVALID_HANDLE) {
			bufferManager->destroyBuffer(indexBuffer);
		}
	}
	vertexBuffer = INVALID_HANDLE;
	indexBuffer  = INVALID_HANDLE;
	indexCount   = 0;
}

void Mesh::setVertices(const std::vector<Vertex> &vertices) {
	if (!vertices.empty()) {
		if (vertexBuffer != INVALID_HANDLE) {
			bufferManager->destroyBuffer(vertexBuffer);
		}
		vertexBuffer = bufferManager->createVertexBuffer(
		    vertices.data(),
		    vertices.size() * sizeof(Vertex));
	}
}

void Mesh::setIndices(const std::vector<uint32_t> &indices) {
	if (!indices.empty()) {
		if (indexBuffer != INVALID_HANDLE) {
			bufferManager->destroyBuffer(indexBuffer);
		}
		indexBuffer = bufferManager->createIndexBuffer(
		    indices.data(),
		    indices.size() * sizeof(uint32_t));
		indexCount = static_cast<uint32_t>(indices.size());
	}
}

void Mesh::bind(VkCommandBuffer cmd) const {
	if (vertexBuffer != INVALID_HANDLE) {
		VkBuffer     vkVertexBuffer = bufferManager->getVkBuffer(vertexBuffer);
		VkDeviceSize offsets[]      = {0};
		vkCmdBindVertexBuffers(cmd, 0, 1, &vkVertexBuffer, offsets);
	}

	if (indexBuffer != INVALID_HANDLE) {
		VkBuffer vkIndexBuffer = bufferManager->getVkBuffer(indexBuffer);
		vkCmdBindIndexBuffer(cmd, vkIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}
}

void Mesh::draw(VkCommandBuffer cmd) const {
	if (indexCount > 0) {
		vkCmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);
	}
}

bool Mesh::isValid() const {
	return vertexBuffer != INVALID_HANDLE &&
	       indexBuffer != INVALID_HANDLE &&
	       indexCount > 0;
}

void Mesh::upload(const MeshData &data, BufferManager &bufferManager) {
    if (data.vertices.empty() || data.indices.empty()) {
        throw std::runtime_error("[Mesh] : MeshData está vazio!");
    }
    
    // Criar vertex buffer
    vertexBuffer = bufferManager.createVertexBuffer(
        data.vertices.data(),
        data.vertices.size() * sizeof(Vertex)
    );
    
    // Criar index buffer
    indexBuffer = bufferManager.createIndexBuffer(
        data.indices.data(),
        data.indices.size() * sizeof(uint32_t)
    );
    
    indexCount = static_cast<uint32_t>(data.indices.size());
    
    std::cout << "[Mesh] : Upload completo - " 
              << data.vertices.size() << " vértices, "
              << data.indices.size() << " índices" << std::endl;
}



MeshData MeshFactory::makeTriangle() {
    MeshData data;
    data.vertices = {
        {{ 0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
    };
    data.indices = {0, 1, 2};
    return data;
}

MeshData MeshFactory::makeQuad() {
    MeshData data;
    data.vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 0.0f}}
    };
    data.indices = {0, 1, 2, 2, 3, 0};
    return data;
}

MeshData MeshFactory::makeCube() {
    MeshData data;
    data.vertices = {
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}}, // 0
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}}, // 1
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}, // 2
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}}, // 3
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}}, // 4
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}}, // 5
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}, // 6
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 0.0f}}  // 7
    };
    data.indices = {
        0, 1, 2, 2, 3, 0, // Frente
        1, 5, 6, 6, 2, 1, // Direita
        5, 4, 7, 7, 6, 5, // Trás
        4, 0, 3, 3, 7, 4, // Esquerda
        3, 2, 6, 6, 7, 3, // Topo
        4, 5, 1, 1, 0, 4  // Base
    };
    return data;
}