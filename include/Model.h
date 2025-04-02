#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Common.h"
#include "Mesh.h"
#include "Material.h"

// model�� ���ܿ��� ����� �Ȱ�ġ�� �ϰ�
// ��������� �޽��� �Ȱ�ġ�� �����ȴ� ����, map�� ������ ����ִ´�.
// material�� ������������, default�� 0���� mesh�� ��Ī��Ų��.

struct MapSets {
	std::unordered_map<uint32_t, std::unique_ptr<Mesh> > meshMap;
	std::unordered_map<uint32_t, std::unique_ptr<Material> > materialMap;
};

 class Model {
public:
	static std::shared_ptr<Model> createModel(VulkanContext* context, MapSets& mapSets, std::string path);
	~Model();
private:
	VulkanContext* context;
	std::vector<uint32_t> m_meshes;
	std::vector<uint32_t> m_materials;

	bool init(VulkanContext* context, MapSets& mapSets, std::string path);
	uint32_t loadMaterial(const aiScene* scene, aiMaterial* material, std::string path, MapSets& mapsets);
	void cleanup();
};