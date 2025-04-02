#pragma once

#include "Common.h"
#include "Texture.h"

struct Albedo
{
	glm::vec3 albedo = glm::vec3(1.0f);
	std::shared_ptr<Texture> albedoTexture = nullptr;
	bool flag = false;
};

struct NormalMap
{
	std::shared_ptr<Texture> normalTexture = nullptr;
	bool flag = false;
};

struct RoughnessMetallic {
	float roughness = 0.5f;
	float metallic = 0.0f;
	std::shared_ptr<Texture> roughnessMetallicTexture = nullptr;
	bool flag = false;

};

struct OcclusionMap
{
	float strength = 1.0f;
	std::shared_ptr<Texture> aoTexture = nullptr;
	bool flag = false;
};


struct HeightMap
{
	float height = 0.0f;
	std::shared_ptr<Texture> heightTexture = nullptr;
	bool flag = false;
};

struct Emissive {
	std::shared_ptr<Texture> emissiveTexture = nullptr;
	glm::vec3 emissiveFactor = glm::vec3(0.0f);
	bool flag = false;
};


enum class AlphaMode {
	OPAQUE,
	MASK,
	BLEND
};

struct Transparency {
	AlphaMode alphaMode = AlphaMode::OPAQUE;
	float alphaCutoff = 0.5f;
	bool doubleSided = false;
};

class Material {
public:
	static std::shared_ptr<Material> createMaterial(Albedo& albedo, NormalMap& normalMap, RoughnessMetallic& roughnessMetallic,
		OcclusionMap& ocMap, HeightMap& heightMap, Emissive& emissive, Transparency& transparency);
	~Material();
	Albedo& getAlbedo() { return m_albedo; }
	NormalMap& getNormalMap() { return m_normalMap; }
	RoughnessMetallic& getRoughnessMetallic() { return m_roughnessMetallic; }
	OcclusionMap& getOcclusionMap() { return m_occlusionMap; }
	HeightMap& getHeightMap() { return m_heightMap; }
	Emissive& getEmissive() { return m_emissive; }
	Transparency& getTransparency() { return m_transparency; }
	uint32_t getID() { return m_id; }
private:
	Albedo m_albedo;
	NormalMap m_normalMap;
	RoughnessMetallic m_roughnessMetallic;
	OcclusionMap m_occlusionMap;
	HeightMap m_heightMap;
	Emissive m_emissive;
	Transparency m_transparency;
	uint32_t m_id;

	void cleanup();
	void init(Albedo& albedo, NormalMap& normalMap, RoughnessMetallic& roughnessMetallic,
		OcclusionMap& ocMap, HeightMap& heightMap, Emissive& emissive, Transparency& transparency);
};