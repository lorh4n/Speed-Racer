#pragma once 

#include <core/ResourceTypes.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

struct aiMesh;
struct aiNode;
struct aiScene;

class ModelLoader {
public:
   static std::vector<MeshData> load(const std::string& path);

private:
   static MeshData processMesh(const aiMesh* mesh);
   static void processNode (const aiNode* node, const aiScene* scene, std::vector<MeshData>& outMeshes);

};