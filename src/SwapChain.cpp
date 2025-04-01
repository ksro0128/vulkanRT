#include "include/SwapChain.h"


std::unique_ptr<SwapChain> SwapChain::createSwapChain(GLFWwindow* window, VulkanContext* context) {
	std::unique_ptr<SwapChain> swapChain = std::unique_ptr<SwapChain>(new SwapChain());
	swapChain->init(window, context);
	return swapChain;
}

void SwapChain::init(GLFWwindow* window, VulkanContext* context) {
	initSwapChain(window, context);
	initImageViews(context);
}

SwapChain::~SwapChain() {
	cleanup();
}

void SwapChain::cleanup() {
	std::cout << "SwapChain::cleanup()" << std::endl;
}

void SwapChain::initSwapChain(GLFWwindow* window, VulkanContext* context) {
    // GPU�� surface�� �����ϴ� SwapChain ���� �ҷ�����
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(context);

    // ���ǽ� ���� ����
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    // ���������̼� ��� ����
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    // ���� ���� ���� (���� ü���� �̹��� �ػ� ����)
    VkExtent2D extent = chooseSwapExtent(window, swapChainSupport.capabilities);

    // ���� ü�ο��� �ʿ��� �̹��� �� ���� (�ּ� �̹��� �� + 1)
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    // ���� �ʿ��� �̹��� ���� �ִ��� ������ clamp
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    // SwapChain ���� ����
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = context->getSurface();

    // �̹��� ������ �ּڰ� ���� (�ִ��� �Ʊ� �ô� �̹��� �ִ����� ��)
    createInfo.minImageCount = imageCount;
    // �̹��� ���� �Է�
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; // �� ���� �������� n ���� ����� �����. (���׷��� 3D, cubemap �̿�� ���� �� ���̾� ���)
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // �⺻ ���������� ����ϴ� �÷��� (���� ������ �� 2�� ���� �ʿ�� �ٸ� �÷��� ���)

    // GPU�� �����ϴ� ť�йи� ��� ��������
    QueueFamilyIndices indices = findQueueFamilies(context);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    // ť�йи� ���� ���
    if (indices.graphicsFamily != indices.presentFamily) {
        // [�׷��Ƚ� ť�йи��� ������Ʈ ť�йи��� ���� �ٸ� ť�� ���]
        // �ϳ��� �̹����� �� ť�йи��� ���ÿ� ������ �� �ְ� �Ͽ� ������ ���δ�.
        // (ť�йи��� �̹����� ������ �� �ٸ� ť�йи��� �����ߴ��� Ȯ���ϴ� ���� ����)
        // �׷��� ������ ���ÿ� �����ϸ� �� �ǹǷ� ������ ���α׷��Ӱ� ���� �����ؾ� �Ѵ�. 
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; // ���� ���� ���
        createInfo.queueFamilyIndexCount = 2;                     // ť�йи� ���� ���
        createInfo.pQueueFamilyIndices = queueFamilyIndices;      // ť�йи� �ε��� �迭 ���
    }
    else {
        // [�׷��Ƚ� ť�йи��� ������Ʈ ť�йи��� ���� ���� ť�� ���]
        // ��ó�� 1���� ť�йи��� �����ϹǷ� ť�йи��� �̹��� ������ ����Ѵ�.
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;  // ť�йи��� ���� ���� ���
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;  // ���� ü�� �̹����� ȭ�鿡 ǥ���Ҷ� ����Ǵ� ��ȯ ���
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;  // ������ ����� ���� ���� window���� �����ų�� ���� (����� ���ĺ��� ���� ����)
    createInfo.presentMode = presentMode; // ������Ʈ ��� ����
    createInfo.clipped = VK_TRUE; // ���� ��ǻ�� ȭ�鿡 ������ �ʴ� �κ��� ������ �� ������ ���� (VK_TRUE = ������ ���� �ʰڴ�)

    createInfo.oldSwapchain = VK_NULL_HANDLE; // ��Ȱ���� ���� ����ü�� ���� (���� �����Ѵٸ� ���ο� �Ҵ��� ���� �ʰ� �����Ѹ�ŭ ���� ����ü�� ���ҽ� ��Ȱ��)

    /*
    [���� ü�� ����]
    ���� ü�� ������ �̹����鵵 ������� ���������,
    ���� �������� �ʿ��� �߰� �̹����� ������ ���� ������ ��
    */
    if (vkCreateSwapchainKHR(context->getDevice(), &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    // ���� ü�� �̹��� ���� ����
    vkGetSwapchainImagesKHR(context->getDevice(), swapChain, &imageCount, nullptr);
    // ���� ü�� �̹��� ������ŭ vector �ʱ�ȭ 
    swapChainImages.resize(imageCount);
    // �̹��� ������ŭ vector�� ���� ü���� �̹��� �ڵ� ä��� 
    vkGetSwapchainImagesKHR(context->getDevice(), swapChain, &imageCount, swapChainImages.data());

    // ���� ü���� �̹��� ���� ����
    swapChainImageFormat = surfaceFormat.format;
    // ���� ü���� �̹��� ũ�� ����
    swapChainExtent = extent;
}


/*
GPU�� �����ϴ� ť�йи� �ε��� ��������
�׷��Ƚ� ť�йи�, ���������̼� ť�йи� �ε����� ����
�ش� ť�йи��� ������ optional ��ü�� ������ empty ����
*/
QueueFamilyIndices SwapChain::findQueueFamilies(VulkanContext* context) {
	QueueFamilyIndices indices;

	// GPU�� �����ϴ� ť �йи� ���� ��������
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(context->getPhysicalDevice(), &queueFamilyCount, nullptr);

	// GPU�� �����ϴ� ť �йи� ����Ʈ ��������
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(context->getPhysicalDevice(), &queueFamilyCount, queueFamilies.data());

	// �׷��� ť �йи� �˻�
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		// �׷��� ť �йи� ã�� ������ ��� indices�� �� ����
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		// GPU�� i �ε��� ť �йи��� surface���� ���������̼��� �����ϴ��� Ȯ��
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(context->getPhysicalDevice(), i, context->getSurface(), &presentSupport);

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


// GPU�� surface�� ȣȯ�ϴ� SwapChain ������ ��ȯ
SwapChainSupportDetails SwapChain::querySwapChainSupport(VulkanContext* context) {
	SwapChainSupportDetails details;

	// GPU�� surface�� ȣȯ�� �� �ִ� capability ���� ����
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->getPhysicalDevice(), context->getSurface(), &details.capabilities);

	// device���� surface ��ü�� �����ϴ� format�� �����ϴ��� Ȯ�� 
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(context->getPhysicalDevice(), context->getSurface(), &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(context->getPhysicalDevice(), context->getSurface(), &formatCount, details.formats.data());
	}

	// device���� surface ��ü�� �����ϴ� presentMode�� �ִ��� Ȯ�� 
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(context->getPhysicalDevice(), context->getSurface(), &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(context->getPhysicalDevice(), context->getSurface(), &presentModeCount, details.presentModes.data());
	}

	return details;
}

/*
�����ϴ� ������ ��ȣ�ϴ� ���� 1�� ��ȯ
��ȣ�ϴ� ������ ���� �� ���� �տ� �ִ� ���� ��ȯ
*/
VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		// ���� ��ȣ�ϴ� ������ ������ ��� �� ������ ��ȯ
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	// ��ȣ�ϴ� ������ ���� ��� ù ��° ���� ��ȯ
	return availableFormats[0];
}


/*
�����ϴ� ���������̼� ��� �� ��ȣ�ϴ� ��� ����
��ȣ�ϴ� ��尡 ���� �� �⺻ ���� VK_PRESENT_MODE_FIFO_KHR ��ȯ
*/
VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		// ��ȣ�ϴ� mode�� �����ϸ� �ش� mode ��ȯ 
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	// ��ȣ�ϴ� mode�� �������� ������ �⺻ ���� VK_PRESENT_MODE_FIFO_KHR ��ȯ
	return VK_PRESENT_MODE_FIFO_KHR;
}


// ���� ü�� �̹����� �ػ� ����
VkExtent2D SwapChain::chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
	// ���� currentExtent�� width�� std::numeric_limits<uint32_t>::max()�� �ƴϸ�, �ý����� �̹� �����ϴ� ����ü�� ũ�⸦ �����ϴ� ��
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent; // ���� ���� ���
	}
	else {
		// �׷��� ���� ���, â�� ���� ������ ���� ũ�⸦ ����Ͽ� ����ü�� ũ�⸦ ����
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		// width, height�� capabilites�� min, max ������ clamping
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

/*
[�̹��� �� ����]
�̹��� ��� VKImage�� ���� ���� ����� �����ϴ� ��ü
GPU�� �̹����� ���� ���� ��� (�̹����� �ؽ�ó�� ���ų� ��ó�� �ϴ� ��)
GPU�� �̹����� ���� �۾��� ���� x
*/
void SwapChain::initImageViews(VulkanContext* context) {
	// �̹����� ������ŭ vector �ʱ�ȭ
	swapChainImageViews.resize(swapChainImages.size());

	// �̹����� ������ŭ �̹����� ����
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		swapChainImageViews[i] = VulkanUtil::createImageView(context, swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}