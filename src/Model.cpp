#include "include/Model.h"
#include <filesystem>

std::shared_ptr<Model> Model::createModel(VulkanContext* context, MapSets& mapSets, std::string path) {
	std::shared_ptr<Model> model = std::shared_ptr<Model>(new Model());
	if (!(model->init(context, mapSets, path)))
		return nullptr;
	return model;
}

bool Model::init(VulkanContext* context, MapSets& mapSets, std::string path) {
	std::filesystem::path p(path);

	if (p.extension() != ".gltf" && p.extension() != ".glb") {
		std::cout << "Model::init" << std::endl;
		return false;
	}

	Assimp::Importer importer;
	const aiScene* scene =
		importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices |
			aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Failed to load GLTF model!" << std::endl;
		return false;
	}

	std::vector<uint32_t> materials(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		materials[i] = loadMaterial(scene, scene->mMaterials[i], path, mapSets);
	}


	return true;
}

uint32_t Model::loadMaterial(const aiScene* scene, aiMaterial* material, std::string path, MapSets& mapsets) {
	Albedo albedo;
	NormalMap normalMap;
	RoughnessMetallic roughnessMetallic;
	OcclusionMap occlusionMap;
	HeightMap heightMap;
	Emissive emissive;
	Transparency transparency;

	
	return 0;
}



Model::~Model() {
	cleanup();
}

void Model::cleanup() {

}