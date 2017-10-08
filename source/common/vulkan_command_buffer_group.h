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
    explicit VulkanCommandBufferGroup(VkDevice device, const uint32_t queueFamilyIndex, const uint32_t commandBufferCount) :
        VulkanCommandBufferGroup(device, queueFamilyIndex, commandBufferCount, 0)
    {}

    explicit VulkanCommandBufferGroup(VkDevice device, const uint32_t queueFamilyIndex, const uint32_t commandBufferCount,
        const VkCommandPoolCreateFlags commandPoolCreateFlags) :
        device(device),
        commandBufferCount(commandBufferCount),
        commandPoolCreateFlags(commandPoolCreateFlags)
    {
        const VkCommandPoolCreateInfo commandPoolCreateInfo =
        {
            VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            nullptr,
            commandPoolCreateFlags,
            queueFamilyIndex
        };

        checkVulkanError(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool), "vkCreateCommandPool");
        commandBuffers.resize(commandBufferCount);
        allocateCommandBuffers();
    }

    ~VulkanCommandBufferGroup()
    {
        vkDestroyCommandPool(device, commandPool, nullptr);
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

    uint32_t getCommandBufferCount() const
    {
        return commandBufferCount;
    }

    VkCommandPoolCreateFlags getCommandPoolCreateFlags() const
    {
        return commandPoolCreateFlags;
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
    uint32_t commandBufferCount;
    VkCommandPoolCreateFlags commandPoolCreateFlags;
    VkCommandPool commandPool;
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
