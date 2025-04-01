#include "include/VulkanContext.h"

std::unique_ptr<VulkanContext> VulkanContext::createVulkanContext(GLFWwindow* window) {
	std::unique_ptr<VulkanContext> context = std::unique_ptr<VulkanContext>(new VulkanContext());
	context->init(window);
	return context;
}

VulkanContext::~VulkanContext() {
	cleanup();
}

void VulkanContext::init(GLFWwindow* window) {
	std::cout << "VulkanContext::init" << std::endl;
	createInstance();
	setupDebugMessenger();
	createSurface(window);
	pickPhysicalDevice();
	createLogicalDevice();
	createCommandPool();
	createDescriptorPool();
}


void VulkanContext::cleanup() {
	std::cout << "VulkanContext::cleanup" << std::endl;
}

void VulkanContext::createInstance() {
	// ����� ��忡�� ���� ���̾� ���� �Ұ��ɽ� ���� �߻�
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	// ���ø����̼� ������ ���� ����ü
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// �ν��Ͻ� ������ ���� ������ ���� ����ü
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	std::vector<const char*> extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	// ����� �޽��� ��ü ������ ���� ���� ����ü
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	if (enableValidationLayers) {
		// ����� ���� ����ü�� ���� ���̾� ����
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		// �ν��Ͻ� ���� �� �ı��ÿ��� ���� ����
		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo);
	}
	else {
		// ����� ��� �ƴ� �� ���� ���̾� x
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	// �ν��Ͻ� ����
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

// ���� ���̾ ��� ������ ���̾� ��Ͽ� �ִ��� Ȯ��
bool  VulkanContext::checkValidationLayerSupport() {
	// Vulkan �ν��Ͻ����� ��� ������ ���̾�� ��� ����
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// �ʿ��� ���� ���̾���� ��� ���� ���̾ ���� �Ǿ��ִ��� Ȯ��
	for (const char* layerName : validationLayers) {
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				// ���� Ȯ��!
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			return false;  // �ʿ��� ���̾ ���ٸ� false ��ȯ
		}
	}
	return true;  // ��� ���̾ �����Ǹ� true ��ȯ
}

//vkDebugUtilsMessengerCreateInfoEXT ����ü ���θ� ä���ִ� �Լ�
void VulkanContext::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

/*
GLFW ���̺귯������ Vulkan �ν��Ͻ��� ������ �� �ʿ��� �ν��Ͻ� Ȯ�� ����� ��ȯ
(����� ���� �޽��� �ݹ� Ȯ�� �߰�)
*/
std::vector<const char*> VulkanContext::getRequiredExtensions() {
	// �ʿ��� Ȯ�� ��� ��������
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	// ����� ����̸� VK_EXT_debug_utils Ȯ�� �߰� (�޼��� �ݹ� Ȯ��)
	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

// ����� �޽��� ��ü ����
void VulkanContext::setupDebugMessenger() {
	// ����� ��� �ƴϸ� return
	if (!enableValidationLayers) return;

	// VkDebugUtilsMessengerCreateInfoEXT ����ü ����
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	// ����� �޽��� ��ü ����
	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}


void VulkanContext::createSurface(GLFWwindow* window) {
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}
}



// ������ GPU ���� �Լ�
void VulkanContext::pickPhysicalDevice() {
	// GPU ��ġ ��� �ҷ�����
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	// ������ GPU Ž��
	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			maxMsaaSamples = getMaxUsableSampleCount();
			break;
		}
	}

	// ������ GPU�� �߰ߵ��� ���� ��� ���� �߻�
	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

// �׷���, ���������̼� ť �йи��� �����ϴ���, GPU�� surface�� ȣȯ�ϴ� SwapChain�� �����ϴ��� �˻�
bool VulkanContext::isDeviceSuitable(VkPhysicalDevice device) {
	// ť �йи� Ȯ��
	QueueFamilyIndices indices = findQueueFamilies(device);

	// ���� ü�� Ȯ���� �����ϴ��� Ȯ��
	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = false;

	// ���� ü�� Ȯ���� �����ϴ� ���
	if (extensionsSupported) {
		// ���� ����̽��� surface�� ȣȯ�ϴ� SwapChain ������ ������
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		// GPU�� surface�� �����ϴ� format�� presentMode�� �����ϸ� ���
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	// GPU ���� �̹漺 ���͸��� �����ϴ��� Ȯ��
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

// GPU ���� �����ϴ� �ִ� ���� ���� ��ȯ
VkSampleCountFlagBits VulkanContext::getMaxUsableSampleCount() {
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

	// GPU�� �����ϴ� color ���ø� ������ depth ���ø� ������ ���� �и� ã��
	VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

	// ���� ���� ���ø� �������� Ȯ���ϸ鼭 ��ȯ
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

/*
GPU�� �����ϴ� ť�йи� �ε��� ��������
�׷��Ƚ� ť�йи�, ���������̼� ť�йи� �ε����� ����
�ش� ť�йи��� ������ optional ��ü�� ������ empty ����
*/
QueueFamilyIndices VulkanContext::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	// GPU�� �����ϴ� ť �йи� ���� ��������
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	// GPU�� �����ϴ� ť �йи� ����Ʈ ��������
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	// �׷��� ť �йи� �˻�
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		// �׷��� ť �йи� ã�� ������ ��� indices�� �� ����
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		// GPU�� i �ε��� ť �йи��� surface���� ���������̼��� �����ϴ��� Ȯ��
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		// ���������̼� ť �йи� ���
		if (presentSupport) {
			indices.presentFamily = i;
		}

		// �׷��� ť �йи� ã�� ��� break
		if (indices.isComplete()) {
			break;
		}

		i++;
	}
	// �׷��� ť �йи��� �� ã�� ��� ���� ���� ä�� ��ȯ ��
	return indices;
}

// ����̽��� �����ϴ� Ȯ�� �� 
bool VulkanContext::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// ���� ü�� Ȯ���� �����ϴ��� Ȯ��
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
	for (const auto& extension : availableExtensions) {
		// ���� ������ Ȯ��� ����� ��ȸ�ϸ� ����
		requiredExtensions.erase(extension.extensionName);
	}

	// ���� ������ �ִ� Ȯ���� ���ŵǸ� true
	// ������ �ִ� Ȯ���� �״�θ� ������ �� �ϴ� ���̹Ƿ� false
	return requiredExtensions.empty();
}

// GPU�� surface�� ȣȯ�ϴ� SwapChain ������ ��ȯ
SwapChainSupportDetails VulkanContext::querySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;

	// GPU�� surface�� ȣȯ�� �� �ִ� capability ���� ����
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	// device���� surface ��ü�� �����ϴ� format�� �����ϴ��� Ȯ�� 
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	// device���� surface ��ü�� �����ϴ� presentMode�� �ִ��� Ȯ�� 
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

// GPU�� ������ �������̽��� Logical device ����
void VulkanContext::createLogicalDevice() {
	// �׷��� ť �йи��� �ε����� ������
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	// ť �йи��� �ε������� set���� ����
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	// ť ������ ���� ���� ���� 
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	// ť �켱���� 0.0f ~ 1.0f �� ǥ��
	float queuePriority = 1.0f;
	// ť �йи� ���� ���� ����
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// ����� ��ġ ����� ���Ե� ����ü
	// vkGetPhysicalDeviceFeatures �Լ��� ����̽����� ���� ������
	// ��ġ ��� ����� Ȯ���� �� ����
	// �ϴ� ������ VK_FALSE�� ���� �����
	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;		// �̹漺 ���͸� ��� ����
	deviceFeatures.sampleRateShading = VK_TRUE; 	// ����̽��� ���� ���̵� ��� Ȱ��ȭ

	// ���� ��ġ ������ ���� ���� ���
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;

	// Ȯ�� ����
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	// ������ ȣȯ�� ���� ����� ����� ���
	// ���� ���̾ ���� ��Ű����, ���� �ý��ۿ����� ���� ��ġ�� ���̾ �� ��
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	// ���� ��ġ ����
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	// ť �ڵ� ��������
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

/*
	[Ŀ�ǵ� Ǯ ����]
	Ŀ�ǵ� Ǯ�̶�?
	1. Ŀ�ǵ� ���۵��� �����Ѵ�.
	2. ť �йи��� 1���� Ŀ�ǵ� Ǯ�� �ʿ��ϴ�.
*/
void VulkanContext::createCommandPool() {
	// ť �йи� �ε��� ��������
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; 		// Ŀ�ǵ� ���۸� ���������� �缳���� �� �ֵ��� ���� 
																			// (�̰� �ƴϸ� Ŀ�ǵ� Ǯ�� ��� Ŀ�ǵ� ���� ������ �� ���� �̷����)
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(); 	// �׷��Ƚ� ť �ε��� ��� (������ų ť �йи� ���)

	// Ŀ�ǵ� Ǯ ����
	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}


// ��ũ���� Ǯ ����
void VulkanContext::createDescriptorPool() {
	size_t MAX_OBJECTS = 1000;

	// ��ũ���� Ǯ�� Ÿ�Ժ� ��ũ���� ������ �����ϴ� ����ü
	std::array<VkDescriptorPoolSize, 4> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;							// ������ ���� ����
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * MAX_OBJECTS);		// ������ ���� ��ũ���� �ִ� ���� ����
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;					// ���÷� ����
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * MAX_OBJECTS);		// ���÷� ��ũ���� �ִ� ���� ����
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * MAX_OBJECTS);
	poolSizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[3].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * MAX_OBJECTS);



	// ��ũ���� Ǯ�� ������ �� �ʿ��� ���� ������ ��� ����ü
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());			// ��ũ���� poolSize ����ü ����
	poolInfo.pPoolSizes = poolSizes.data();										// ��ũ���� poolSize ����ü �迭
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * MAX_OBJECTS);				// Ǯ�� ������ �� �ִ� �� ��ũ���� �� ����

	// ��ũ���� Ǯ ����
	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

// ����� �޽��� �ݹ� �Լ�
VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	// �޽��� ���븸 ���
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	// VK_TRUE ��ȯ�� ���α׷� �����
	return VK_FALSE;
}