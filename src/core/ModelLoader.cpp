#include <core/ModelLoader.hpp>

std::vector<MeshData> ModelLoader::load(const std::string &path) {
	std::cout << "[ModelLoader] : Carregando modelo: " << path << std::endl;

	Assimp::Importer importer;

	// Flags importantes:
	// aiProcess_Triangulate → garante que tudo vira triângulo
	// aiProcess_GenNormals → gera normais (pra iluminação futura)
	// aiProcess_JoinIdenticalVertices → otimiza vértices duplicados

	const aiScene *scene = importer.ReadFile(path,
	                                          aiProcess_Triangulate |
	                                          aiProcess_GenNormals |
	                                          aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		throw std::runtime_error(
		    "[ModelLoader] : Erro ao carregar modelo: " +
		    std::string(importer.GetErrorString()));
	}

	std::vector<MeshData> meshes;
	processNode(scene->mRootNode, scene, meshes);

	std::cout << "[ModelLoader] : Carregado com sucesso! "
	          << meshes.size() << " submeshes encontradas." << std::endl;

	return meshes;
}

void ModelLoader::processNode(const aiNode* node, const aiScene* scene, std::vector<MeshData>& outMeshes) {

   for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      outMeshes.push_back(processMesh(mesh));
   }

   // Recursão: processar filhos

   for (unsigned int i = 0; i < node->mNumChildren; i++) {
      processNode(node->mChildren[i], scene, outMeshes);
   }

}

MeshData ModelLoader::processMesh(const aiMesh* mesh) {
    MeshData data;
    
    // --- VÉRTICES ---
    data.vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{};
        
        // Posição (sempre existe)
        vertex.pos[0] = mesh->mVertices[i].x;
        vertex.pos[1] = mesh->mVertices[i].y;
        vertex.pos[2] = mesh->mVertices[i].z;
        
        // Cor (se não tiver, usa cinza)
        if (mesh->HasVertexColors(0)) {
            vertex.color[0] = mesh->mColors[0][i].r;
            vertex.color[1] = mesh->mColors[0][i].g;
            vertex.color[2] = mesh->mColors[0][i].b;
        } else {
            vertex.color[0] = 0.7f;  // Cinza
            vertex.color[1] = 0.7f;
            vertex.color[2] = 0.7f;
        }
        
        data.vertices.push_back(vertex);
    }
    
    // --- ÍNDICES ---
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            data.indices.push_back(face.mIndices[j]);
        }
    }
    
    std::cout << "[ModelLoader] :   Mesh processada - " 
              << data.vertices.size() << " vértices, "
              << data.indices.size() << " índices" << std::endl;
    
    return data;
}