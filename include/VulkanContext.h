#pragma once

#include "Common.h"

class VulkanContext {
public:
	static std::unique_ptr<VulkanContext> createVulkanContext(GLFWwindow* window);
	~VulkanContext();
	void createSurface(GLFWwindow* window);

	VkInstance getInstance() { return instance; }
	VkDebugUtilsMessengerEXT getDebugMessenger() { return debugMessenger; }
	VkPhysicalDevice getPhysicalDevice() { return physicalDevice; }
	VkDevice getDevice() { return device; }
	VkQueue getGraphicsQueue() { return graphicsQueue; }
	VkQueue getPresentQueue() { return presentQueue; }
	VkCommandPool getCommandPool() { return commandPool; }
	VkSurfaceKHR getSurface() { return surface; }
	VkSampleCountFlagBits getMaxMsaaSamples() { return maxMsaaSamples; }
	VkDescriptorPool getDescriptorPool() { return descriptorPool; }

private:
	VulkanContext() {}

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkSampleCountFlagBits maxMsaaSamples = VK_SAMPLE_COUNT_1_BIT;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkCommandPool commandPool;
	VkDescriptorPool descriptorPool;


	void init(GLFWwindow* window);
	void cleanup();
	void createInstance();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	VkSampleCountFlagBits getMaxUsableSampleCount();
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	void createLogicalDevice();
	void createCommandPool();
	void createDescriptorPool();


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
};