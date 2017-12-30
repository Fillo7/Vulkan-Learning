#pragma once

#include "vulkan/vulkan.h"
#include "vulkan_utility.h"

namespace VulkanLearning
{

class VulkanDescriptorPool
{
public:
    explicit VulkanDescriptorPool(VkDevice device) :
        device(device)
    {
        const VkDescriptorPoolSize poolSize =
        {
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            1
        };

        const VkDescriptorPoolCreateInfo poolCreateInfo =
        {
            VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            nullptr,
            0,
            1,
            1,
            &poolSize
        };

        checkVulkanError(vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &descriptorPool), "vkCreateDescriptorPool");
    }

    ~VulkanDescriptorPool()
    {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    }

    VkDevice getDevice() const
    {
        return device;
    }

    VkDescriptorPool getDescriptorPool() const
    {
        return descriptorPool;
    }

private:
    VkDevice device;
    VkDescriptorPool descriptorPool;
};

} // namespace VulkanLearning
