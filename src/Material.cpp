#include "include/Material.h"

std::shared_ptr<Material> Material::createMaterial(Albedo& albedo,  NormalMap& normalMap,
	 RoughnessMetallic& roughnessMetallic,
	 OcclusionMap& ocMap,  HeightMap& heightMap,
	 Emissive& emissive,  Transparency& transparency) {
	std::shared_ptr<Material> material = std::shared_ptr<Material>(new Material());
	material->init(albedo, normalMap, roughnessMetallic, ocMap, heightMap, emissive, transparency);
	static uint32_t id = 0;
	material->m_id = id++;
	return material;
}

void Material::init(Albedo& albedo,  NormalMap& normalMap,
	 RoughnessMetallic& roughnessMetallic,
	 OcclusionMap& ocMap,  HeightMap& heightMap,
	 Emissive& emissive,  Transparency& transparency) {
	m_albedo = albedo;
	m_normalMap = normalMap;
	m_roughnessMetallic = roughnessMetallic;
	m_occlusionMap = ocMap;
	m_heightMap = heightMap;
	m_emissive = emissive;
	m_transparency = transparency;
}

Material::~Material() {
	cleanup();
}

void Material::cleanup() {

}