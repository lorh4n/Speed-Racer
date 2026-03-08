#pragma once

#include <core/Handle.hpp>
#include <core/BufferManager.hpp>
#include <core/ResourceManager.hpp>
#include <vector>

// Dados brutos da mesh (CPU side)
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

class Mesh {
  private:
	BufferHandle   vertexBuffer;
	BufferHandle   indexBuffer;
	uint32_t       indexCount;
	BufferManager *bufferManager;

   bool uploaded = false;

	void cleanup();

  public:
	Mesh(BufferManager *bufferMgr);
	~Mesh();

	// Move Semantics
	Mesh(Mesh &&other) noexcept;
	Mesh &operator=(Mesh &&other) noexcept;

	// Proibir Cópia
	Mesh(const Mesh &)            = delete;
	Mesh &operator=(const Mesh &) = delete;

	void setVertices(const std::vector<Vertex> &vertices);
	void setIndices(const std::vector<uint32_t> &indices);

	void bind(VkCommandBuffer cmd) const;
	void draw(VkCommandBuffer cmd) const;

      // Upload dados para GPU via BufferManager
   void upload(const MeshData& data, BufferManager& bufferManager);

	bool isValid() const;
};

namespace MeshFactory {
    MeshData makeTriangle();
    MeshData makeQuad();
    MeshData makeCube();
    // Futuro: MeshData makePlane(float width, float depth, int subdivisions);
}