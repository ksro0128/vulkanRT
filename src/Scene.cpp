#include "include/Scene.h"

std::unique_ptr<Scene> Scene::createScene(uint32_t maxModelIndex, uint32_t maxMaterialIndex, uint32_t maxTextureIndex) {
	std::unique_ptr<Scene> scene = std::unique_ptr<Scene>(new Scene());
	scene->init(maxModelIndex, maxMaterialIndex, maxTextureIndex);
	return scene;
}

Scene::~Scene() {
}

void Scene::init(uint32_t maxModelIndex, uint32_t maxMaterialIndex, uint32_t maxTextureIndex) {
	this->maxModelIndex = maxModelIndex;
	this->maxMaterialIndex = maxMaterialIndex;
	this->maxTextureIndex = maxTextureIndex;

	m_ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
	{
		Light light;
		light.type = LIGHT_TYPE_DIRECTIONAL;
		light.castsShadow = true;
		light.direction = glm::vec4(-0.5f, -1.0f, -1.0f, 0.0f);
		light.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		light.intensity = 3.0f;
		light.range = 100.0f;
		light.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		light.spotInnerAngle = 30.0f;
		light.spotOuterAngle = 45.0f;
		m_lights.push_back(light);
	}


	{
		Object obj;
		obj.modelIndex = 0;
		obj.position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_objects.push_back(obj);
	}
	

}