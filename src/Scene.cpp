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
		light.range = 10.0f;
		light.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		light.spotInnerAngle = 30.0f;
		light.spotOuterAngle = 45.0f;
		m_lights.push_back(light);
	}


	{
		Object obj;
		obj.modelIndex = 0;
		obj.position = glm::vec3(-1.1f, 1.1f, 0.0f);
		obj.overrideMaterialIndex.push_back(0);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.position = glm::vec3(0.0f, 1.1f, 0.0f);
		obj.overrideMaterialIndex.push_back(1);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.position = glm::vec3(1.1f, 1.1f, 0.0f);
		obj.overrideMaterialIndex.push_back(2);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.position = glm::vec3(-1.1f, 0.0f, 0.0f);
		obj.overrideMaterialIndex.push_back(3);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 11;
		obj.position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 2;
		obj.position = glm::vec3(1.1f, 0.0f, 0.0f);

		obj.overrideMaterialIndex.push_back(5);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 1;
		obj.position = glm::vec3(-1.1f, -1.1f, 0.0f);

		obj.overrideMaterialIndex.push_back(6);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 2;
		obj.position = glm::vec3(0.0f, -1.1f, 0.0f);

		obj.overrideMaterialIndex.push_back(7);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 1;
		obj.position = glm::vec3(1.1f, -1.1f, 0.0f);

		obj.overrideMaterialIndex.push_back(8);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.position = glm::vec3(0.0f, -2.0f, 0.0f);
		obj.rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
		obj.scale = glm::vec3(10.0f);
		m_objects.push_back(obj);
	}

}