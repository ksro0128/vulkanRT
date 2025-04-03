#pragma once

#include "Common.h"
#include "Model.h"


struct Object {
	int32_t modelIndex = -1;
	glm::mat4 transform = glm::mat4(1.0f);
	std::vector<int32_t> overrideMaterialIndex;
};

void printObject(const Object& obj);