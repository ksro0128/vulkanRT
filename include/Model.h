#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Common.h"
#include "Mesh.h"
#include "Material.h"

// model은 윗단에서 절대로 안겹치게 하고
// 만들어지는 메쉬은 안겹치고 생성된다 가정, map에 무조건 집어넣는다.
// material도 마찬가지지만, default면 0으로 mesh와 매칭시킨다.

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