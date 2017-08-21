#pragma once

#include <cstdint>
#include <vector>

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanCommandBuffer
{
public:
    explicit VulkanCommandBuffer(const VkDevice device, const uint32_t queueFamilyIndex, const uint32_t commandBufferCount) :
        device(device)
    {
        const VkCommandPoolCreateInfo commandPoolCreateInfo =
        {
            VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            nullptr,
            0,
            queueFamilyIndex
        };

        checkVulkanError(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool), "vkCreateCommandPool");

        const VkCommandBufferAllocateInfo commandBufferAllocateInfo =
        {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            nullptr,
            commandPool,
            VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            commandBufferCount
        };

        commandBuffers.resize(commandBufferCount);
        checkVulkanError(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data()), "vkAllocateCommandBuffers");
    }

    ~VulkanCommandBuffer()
    {
        vkDestroyCommandPool(device, commandPool, nullptr);
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkCommandPool getCommandPool() const
    {
        return commandPool;
    }

    std::vector<VkCommandBuffer> getCommandBuffers() const
    {
        return commandBuffers;
    }

private:
    VkDevice device;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};

} // namespace VulkanLearning
