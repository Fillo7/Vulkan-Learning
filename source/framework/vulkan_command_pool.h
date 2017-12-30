#pragma once

#include <cstdint>
#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanCommandPool
{
public:
    explicit VulkanCommandPool(VkDevice device, const uint32_t queueFamilyIndex) :
        VulkanCommandPool(device, queueFamilyIndex, 0)
    {}

    explicit VulkanCommandPool(VkDevice device, const uint32_t queueFamilyIndex, const VkCommandPoolCreateFlags commandPoolCreateFlags) :
        device(device),
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
    }

    ~VulkanCommandPool()
    {
        vkDestroyCommandPool(device, commandPool, nullptr);
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkCommandPoolCreateFlags getCommandPoolCreateFlags() const
    {
        return commandPoolCreateFlags;
    }

    VkCommandPool getCommandPool() const
    {
        return commandPool;
    }

private:
    VkDevice device;
    VkCommandPoolCreateFlags commandPoolCreateFlags;
    VkCommandPool commandPool;
};

} // namespace VulkanLearning
