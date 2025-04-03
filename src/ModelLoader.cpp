#include "include/ModelLoader.h"

Model ModelLoader::loadGLTFModel(const std::string& path, VulkanContext* context,
	std::vector<std::unique_ptr<Mesh>>& meshList,
	std::vector<std::unique_ptr<Texture>>& textureList,
	std::vector<Material>& materialList)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw std::runtime_error("Failed to load glTF model: " + path);
	}
	std::filesystem::path basePath = std::filesystem::path(path).parent_path();

	Model model;
	processNode(scene->mRootNode, scene, context, meshList, textureList, materialList, model, basePath);

	return model;
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene, VulkanContext* context,
	std::vector<std::unique_ptr<Mesh>>& meshList,
	std::vector<std::unique_ptr<Texture>>& textureList,
	std::vector<Material>& materialList,
	Model& model,
	const std::filesystem::path& basePath)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		auto newMesh = processMesh(mesh, scene, context);
		int32_t meshIndex = static_cast<int32_t>(meshList.size());
		meshList.push_back(std::move(newMesh));
		model.mesh.push_back(meshIndex);

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];
			Material mat = processMaterial(aiMat, scene, context, textureList, basePath); // basePath ����
			int32_t materialIndex = static_cast<int32_t>(materialList.size());
			materialList.push_back(mat);
			model.material.push_back(materialIndex);
		}
		else
		{
			model.material.push_back(-1);
		}
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene, context, meshList, textureList, materialList, model, basePath);
	}
}



std::unique_ptr<Mesh> ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, VulkanContext* context)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	// ���ؽ� ������
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex{};
		vertex.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		if (mesh->HasTextureCoords(0))
			vertex.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			vertex.texCoord = glm::vec2(0.0f);

		if (mesh->HasTangentsAndBitangents())
			vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		else
			vertex.tangent = glm::vec3(0.0f);

		vertices.push_back(vertex);
	}

	// �ε��� ������
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	return Mesh::createMesh(context, vertices, indices);
}

Material ModelLoader::processMaterial(aiMaterial* aiMat, const aiScene* scene,
	VulkanContext* context,
	std::vector<std::unique_ptr<Texture>>& textureList,
	const std::filesystem::path& basePath)
{
	Material mat{};

	mat.albedoTexIndex = loadTexture(aiMat, aiTextureType_BASE_COLOR, context, textureList, basePath, TextureFormatType::ColorSRGB);
	mat.normalTexIndex = loadTexture(aiMat, aiTextureType_NORMALS, context, textureList, basePath, TextureFormatType::LinearUNORM);
	mat.metallicTexIndex = loadTexture(aiMat, aiTextureType_METALNESS, context, textureList, basePath, TextureFormatType::LinearUNORM);
	mat.roughnessTexIndex = loadTexture(aiMat, aiTextureType_DIFFUSE_ROUGHNESS, context, textureList, basePath, TextureFormatType::LinearUNORM);
	mat.aoTexIndex = loadTexture(aiMat, aiTextureType_AMBIENT_OCCLUSION, context, textureList, basePath, TextureFormatType::LinearUNORM);
	mat.emissiveTexIndex = loadTexture(aiMat, aiTextureType_EMISSIVE, context, textureList, basePath, TextureFormatType::ColorSRGB);

	float metallic = 0.0f, roughness = 0.5f;
	aiMat->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
	aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
	mat.metallic = metallic;
	mat.roughness = roughness;

	aiColor4D baseColor;
	if (aiMat->Get(AI_MATKEY_BASE_COLOR, baseColor) == AI_SUCCESS) {
		mat.baseColor = glm::vec4(baseColor.r, baseColor.g, baseColor.b, baseColor.a);
	}

	return mat;
}


int32_t ModelLoader::loadTexture(aiMaterial* aiMat, aiTextureType type,
	VulkanContext* context,
	std::vector<std::unique_ptr<Texture>>& textureList,
	const std::filesystem::path& basePath, TextureFormatType formatType)
{
	if (aiMat->GetTextureCount(type) > 0)
	{
		aiString texPath;
		if (aiMat->GetTexture(type, 0, &texPath) == AI_SUCCESS)
		{
			std::filesystem::path fullPath = basePath / texPath.C_Str();

			auto texture = Texture::createTexture(context, fullPath.string(), formatType);
			textureList.push_back(std::move(texture));

			return static_cast<int32_t>(textureList.size()) - 1;
		}
	}
	return -1;
}
