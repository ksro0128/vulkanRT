#include "include/Object.h"
#include <iomanip>

void printMatrix(const glm::mat4& mat) {
	std::cout << "Transform Matrix:\n";
	for (int i = 0; i < 4; ++i) {
		std::cout << "  ";
		for (int j = 0; j < 4; ++j) {
			std::cout << std::setw(10) << mat[j][i] << " ";
		}
		std::cout << "\n";
	}
}

void printObject(const Object& obj) {
	std::cout << "=== Object ===\n";
	std::cout << "Model Index         : " << obj.modelIndex << "\n";
	printMatrix(obj.transform);

	std::cout << "Override Materials  : ";
	if (obj.overrideMaterialIndex.empty()) {
		std::cout << "(none)";
	}
	else {
		for (const auto& index : obj.overrideMaterialIndex) {
			std::cout << index << " ";
		}
	}
	std::cout << "\n";
}