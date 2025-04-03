#pragma once

#include "Common.h"
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "VulkanContext.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>


class ModelLoader {
public:
	static Model loadGLTFModel(
		const std::string& path,
		VulkanContext* context,
		std::vector<std::unique_ptr<Mesh>>& meshList,
		std::vector<std::unique_ptr<Texture>>& textureList,
		std::vector<Material>& materialList
	);
private:
	static void processNode(
		aiNode* node, const aiScene* scene,
		VulkanContext* context,
		std::vector<std::unique_ptr<Mesh>>& meshList,
		std::vector<std::unique_ptr<Texture>>& textureList,
		std::vector<Material>& materialList,
		Model& model, const std::filesystem::path& basePath
	);

	static std::unique_ptr<Mesh> processMesh(
		aiMesh* mesh, const aiScene* scene,
		VulkanContext* context
	);

	static Material processMaterial(
		aiMaterial* material, const aiScene* scene,
		VulkanContext* context,
		std::vector<std::unique_ptr<Texture>>& textureList, const std::filesystem::path& basePath
	);

	static int32_t loadTexture(
		aiMaterial* material, aiTextureType type,
		VulkanContext* context,
		std::vector<std::unique_ptr<Texture>>& textureList, const std::filesystem::path& basePath, TextureFormatType formatType
	);

};