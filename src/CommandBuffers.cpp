#include "include/CommandBuffers.h"


std::unique_ptr<CommandBuffers> CommandBuffers::createCommandBuffers(VulkanContext* context) {
	std::unique_ptr<CommandBuffers> commandBuffers = std::unique_ptr<CommandBuffers>(new CommandBuffers());
	commandBuffers->init(context);
	return commandBuffers;
}

void CommandBuffers::init(VulkanContext* context) {
	this->context = context;

	m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	// 커맨드 버퍼 설정값 준비
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = context->getCommandPool();			   // 커맨드 풀 등록
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // 큐에 직접 제출할 수 있는 커맨드 버퍼 설정
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size(); // 할당할 커맨드 버퍼의 개수

	// 커맨드 버퍼 할당
	if (vkAllocateCommandBuffers(context->getDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

CommandBuffers::~CommandBuffers() {
	cleanup();
}

void CommandBuffers::cleanup() {
	vkFreeCommandBuffers(context->getDevice(), context->getCommandPool(), static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
}