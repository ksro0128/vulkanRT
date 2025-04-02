#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>


#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <optional>
#include <set>
#include <unordered_map>


const int MAX_FRAMES_IN_FLIGHT = 2;

class IDGenerator {
public:
	static uint32_t getNextMeshID() {
		static uint32_t meshID = 0;
		return meshID++;
	}

	static uint32_t getNextMaterialID() {
		static uint32_t matID = 0;
		return matID++;
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif



const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};


const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::vec3 tangent;

	// 정점 데이터가 전달되는 방법을 알려주는 구조체 반환하는 함수
	static VkVertexInputBindingDescription getBindingDescription() {
		// 파이프라인에 정점 데이터가 전달되는 방법을 알려주는 구조체
		VkVertexInputBindingDescription bindingDescription{};		
		bindingDescription.binding = 0;								// 버텍스 바인딩 포인트 (현재 0번에 vertex 정보 바인딩)
		bindingDescription.stride = sizeof(Vertex);					// 버텍스 1개 단위의 정보 크기
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // 정점 데이터 처리 방법
																	// 1. VK_VERTEX_INPUT_RATE_VERTEX : 정점별로 데이터 처리
																	// 2. VK_VERTEX_INPUT_RATE_INSTANCE : 인스턴스별로 데이터 처리
		return bindingDescription;
	}

	// 정점 속성별 데이터 형식과 위치를 지정하는 구조체 반환하는 함수
	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		// 정점 속성의 데이터 형식과 위치를 지정하는 구조체
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

		// pos 속성 정보 입력
		attributeDescriptions[0].binding = 0;							// 버텍스 버퍼의 바인딩 포인트
		attributeDescriptions[0].location = 0;							// 버텍스 셰이더의 어떤 location에 대응되는지 지정
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;	// 저장되는 데이터 형식 (VK_FORMAT_R32G32B32_SFLOAT = vec3)
		attributeDescriptions[0].offset = offsetof(Vertex, pos);		// 버텍스 구조체에서 해당 속성이 시작되는 위치

		// color 속성 정보 입력
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, normal);

		// texCoord 속성 정보 입력
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		// tangent 속성 정보 입력
		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, tangent);

		return attributeDescriptions;
	}
};

struct CameraUniformBuffer {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};