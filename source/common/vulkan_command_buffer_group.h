#pragma once

#include <cstdint>
#include <vector>
#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanCommandBufferGroup
{
public:
    explicit VulkanCommandBufferGroup(VkDevice device, VkCommandPool commandPool, const uint32_t commandBufferCount) :
        device(device),
        commandPool(commandPool),
        commandBufferCount(commandBufferCount)
    {
        commandBuffers.resize(commandBufferCount);
        allocateCommandBuffers();
    }

    void destroyCommandBuffers()
    {
        vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    }

    void reloadCommandBuffers()
    {
        allocateCommandBuffers();
    }

    VkDevice getDevice() const
    {
        return device;
    }
    
    VkCommandPool getCommandPool() const
    {
        return commandPool;
    }

    uint32_t getCommandBufferCount() const
    {
        return commandBufferCount;
    }

    std::vector<VkCommandBuffer> getCommandBuffers() const
    {
        return commandBuffers;
    }

private:
    VkDevice device;
    VkCommandPool commandPool;
    uint32_t commandBufferCount;
    std::vector<VkCommandBuffer> commandBuffers;

    void allocateCommandBuffers()
    {
        const VkCommandBufferAllocateInfo commandBufferAllocateInfo =
        {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            nullptr,
            commandPool,
            VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            commandBufferCount
        };

        checkVulkanError(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data()), "vkAllocateCommandBuffers");
    }
};

} // namespace VulkanLearning
